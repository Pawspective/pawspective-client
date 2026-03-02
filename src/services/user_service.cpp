#include "services/user_service.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>

#include "models/user_dto.hpp"
#include "services/errors.hpp"

namespace pawspective::services {

UserService::UserService(NetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void UserService::handleError(QNetworkReply& reply) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit requestFailed(
            QSharedPointer<BaseError>(new ClientJsonParseError(
                QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
            ))
        );
        return;
    }

    if (doc.isObject()) {
        auto error = ErrorFactory::createError(doc.object());
        emit requestFailed(QSharedPointer<BaseError>(std::move(error)));
    } else {
        emit requestFailed(QSharedPointer<BaseError>(new UnknownError("Unknown error occurred")));
    }
}

void UserService::handleSuccess(QNetworkReply& reply, std::function<void(const QJsonObject&)> onSuccess) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
    try {
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            emit requestFailed(
                QSharedPointer<BaseError>(new class ClientJsonParseError(
                    QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
                ))
            );
            return;
        }

        onSuccess(doc.object());
    } catch (const std::exception& e) {
        emit requestFailed(QSharedPointer<BaseError>(new ClientJsonParseError(QString(e.what()))));
    }
}

void UserService::updateUserProfile(const models::UserUpdateDTO& dto) {
    const QJsonDocument data(dto.toJson());
    if (!m_networkClient.getUserId()) {
        emit requestFailed(QSharedPointer<BaseError>(new ValidationError("User ID is not set")));
        return;
    }
    m_networkClient.put(
        QUrl(QString("/update/%1").arg(*m_networkClient.getUserId())),
        data.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(reply, [this](const QJsonObject& obj) {
                models::UserDTO user = models::UserDTO::fromJson(obj);
                emit updateUserProfileSuccess(user);
            });
        },
        [this](QNetworkReply& reply) { handleError(reply); }
    );
}

void UserService::registerUser(const models::UserRegisterDTO& dto) {
    const QJsonDocument doc(dto.toJson());
    m_networkClient.post(
        QUrl("/user/register"),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(reply, [this](const QJsonObject& obj) {
                models::UserDTO user = models::UserDTO::fromJson(obj);
                emit registerUserSuccess(user);
            });
        },
        [this](QNetworkReply& reply) { handleError(reply); }
    );
}

}  // namespace pawspective::services
