#include "services/auth_service.hpp"
#include <QByteArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QUrl>
#include <optional>
#include "services/errors.hpp"
#include "services/network_client.hpp"
#include "validator.hpp"

namespace {
std::optional<QJsonObject> parseJwtPayload(const QString& token) {
    QStringList parts = token.split('.');
    if (parts.size() == 3) {
        return std::nullopt;
    }

    QByteArray payload = parts[1].toUtf8();
    QString base64Payload = QString::fromUtf8(payload);
    base64Payload = base64Payload.replace('-', '+').replace('_', '/');

    while (base64Payload.size() % 4 != 0) {
        base64Payload.append('=');
    }

    QByteArray decoded = QByteArray::fromBase64(base64Payload.toUtf8());
    QJsonDocument doc = QJsonDocument::fromJson(decoded);

    if (!doc.isObject()) {
        return std::nullopt;
    }

    return doc.object();
}

std::optional<int> extractTokenExpiration(const QString& token) {
    auto payload = parseJwtPayload(token);
    if (!payload.has_value()) {
        return std::nullopt;
    }

    QJsonObject obj = payload.value();

    if (obj.contains("exp")) {
        qint64 exp = obj["exp"].toVariant().toLongLong();
        qint64 now = QDateTime::currentSecsSinceEpoch();

        int secondsLeft = static_cast<int>(exp - now);

        return secondsLeft;
    }

    return std::nullopt;
}
std::optional<uint64_t> extractUserIdFromToken(const QString& token) {
    auto payload = parseJwtPayload(token);
    if (!payload.has_value()) {
        return std::nullopt;
    }

    QJsonObject obj = payload.value();

    if (obj.contains("sub")) {
        if (obj["sub"].isString()) {
            return obj["sub"].toString().toULongLong();
        } else if (obj["sub"].isDouble()) {
            return static_cast<uint64_t>(obj["sub"].toDouble());
        }
    }

    return std::nullopt;
}
}  // namespace

namespace pawspective::services {

AuthService::AuthService(NetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient), m_userId(std::nullopt) {
    connect(&m_networkClient, &NetworkClient::unauthorizedAccess, this, &AuthService::handleUnauthorizedAccess);
    connect(&m_networkClient, &NetworkClient::invalidTokenDetected, this, [this]() { clearSession(); });
    m_networkClient.setTokenProvider([this]() { return m_accessToken; });
    m_networkClient.setUserId(m_userId);
}

void AuthService::clearSession() {
    m_isRefreshing = false;
    m_accessToken.clear();
    m_refreshToken.clear();
    m_userId = std::nullopt;
    m_networkClient.setUserId(std::nullopt);
    m_networkClient.clearPendingRequests();
    emit sessionEnded();
}

void AuthService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
    QJsonParseError parseError;

    QByteArray data = reply.property("responseData").toByteArray();
    if (data.startsWith("Network error")) {
        onError(QSharedPointer<UnknownError>::create(QString::fromUtf8(data)));
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        auto error = QSharedPointer<BaseError>(new ClientJsonParseError(
            QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
        ));
        onError(error);
        return;
    }

    if (doc.isObject()) {
        auto error = ErrorFactory::createError(doc.object());
        onError(error);
    }
}
void AuthService::handleSuccess(
    QNetworkReply& reply,
    std::function<void(const QJsonObject&)> onSuccess,
    std::function<void(QSharedPointer<BaseError>)> onError
) {
    QJsonParseError parseError;
    QByteArray data = reply.property("responseData").toByteArray();

    try {
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            auto error = QSharedPointer<BaseError>(new ClientJsonParseError(
                QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
            ));
            onError(error);
            return;
        }

        onSuccess(doc.object());
    } catch (const std::exception& e) {
        auto error = QSharedPointer<BaseError>(new ClientJsonParseError(QString(e.what())));
        onError(error);
    }
}

void AuthService::login(const QString& email, const QString& password) {
    pawspective::utils::Validator validator;
    validator.field("email", email.toStdString()).validateEmail();
    validator.field("password", password.toStdString()).notBlank();

    if (auto validationError = validator.getValidationError()) {
        emit loginFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*validationError))));
        return;
    }

    QUrl url("/auth/login");
    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);

    m_networkClient.post(
        url,
        data,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    auto [accessToken, refreshToken, tokenType] = parseTokenResponse(obj);

                    m_accessToken = accessToken;
                    m_refreshToken = refreshToken;

                    auto userId = extractUserIdFromToken(accessToken);
                    if (userId.has_value()) {
                        uint64_t id = userId.value();
                        m_userId = id;
                        m_networkClient.setUserId(id);
                    }

                    emit loginSuccess(accessToken, refreshToken, tokenType, userId.value_or(0));
                },
                [this](QSharedPointer<BaseError> error) { emit loginFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit loginFailed(error); });
        }
    );
}

void AuthService::logout() {
    QUrl url("/auth/logout");

    QJsonObject json;
    json["refresh_token"] = m_refreshToken;

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);

    m_networkClient.post(
        url,
        std::move(data),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject&) { clearSession(); },
                [this](QSharedPointer<BaseError> error) { emit logoutFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit logoutFailed(error); });
        }
    );
}

void AuthService::refreshToken(const QString& refreshToken) {
    if (m_isRefreshing) {
        return;
    }

    m_isRefreshing = true;

    pawspective::utils::Validator validator;
    validator.field("refresh_token", refreshToken.toStdString()).notBlank();

    if (auto validationError = validator.getValidationError()) {
        emit refreshFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*validationError))));
        m_isRefreshing = false;
        return;
    }

    QUrl url("/auth/refresh");
    QJsonObject json;
    json["refresh_token"] = refreshToken;

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);

    m_networkClient.post(
        url,
        data,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    auto [accessToken, refreshToken, tokenType] = parseTokenResponse(obj);

                    m_accessToken = accessToken;
                    m_refreshToken = refreshToken;

                    auto userId = extractUserIdFromToken(accessToken);
                    if (userId.has_value()) {
                        uint64_t id = userId.value();
                        m_userId = id;
                        m_networkClient.setUserId(id);
                    }

                    m_isRefreshing = false;
                    m_networkClient.retryPendingRequests();

                    emit refreshSuccess(accessToken, refreshToken, tokenType);
                },
                [this](QSharedPointer<BaseError> error) {
                    emit refreshFailed(error);
                    clearSession();
                }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) {
                emit refreshFailed(error);
                clearSession();
            });
        }
    );
}

void AuthService::getCurrentUser() {
    QUrl url("/auth/me");
    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::UserDTO user = models::UserDTO::fromJson(obj);

                    if (user.id > 0) {
                        m_userId = user.id;
                        m_networkClient.setUserId(user.id);
                    }

                    emit getCurrentUserSuccess(user);
                },
                [this](QSharedPointer<BaseError> error) { emit getCurrentUserFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getCurrentUserFailed(error); });
        }
    );
}

void AuthService::handleUnauthorizedAccess() {
    if (!m_refreshToken.isEmpty() && !m_isRefreshing) {
        refreshToken(m_refreshToken);
    }
}

bool AuthService::isAuthenticated() const {
    if (m_accessToken.isEmpty()) {
        return false;
    }

    auto expiresIn = extractTokenExpiration(m_accessToken);
    if (!expiresIn.has_value() || expiresIn.value() <= 0) {
        return false;
    }

    return true;
}

std::tuple<QString, QString, QString> AuthService::parseTokenResponse(const QJsonObject& obj) {
    QString accessToken;
    QString newRefreshToken;
    QString tokenType = "Bearer";

    if (obj.contains("access_token")) {
        accessToken = obj["access_token"].toString();
    } else if (obj.contains("token")) {
        accessToken = obj["token"].toString();
    }

    if (obj.contains("refresh_token")) {
        newRefreshToken = obj["refresh_token"].toString();
    }

    if (obj.contains("token_type")) {
        tokenType = obj["token_type"].toString();
    }

    return {accessToken, newRefreshToken, tokenType};
}

}  // namespace pawspective::services