#include "services/user_service.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>

#include "models/user_dto.hpp"
#include "services/errors.hpp"
#include "validator.hpp"

namespace pawspective::services {

UserService::UserService(NetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void UserService::handleError(QNetworkReply& reply) {
    QByteArray data = reply.property("responseData").toByteArray();

    if (data.isEmpty()) {
        emit requestFailed(QSharedPointer<UnknownError>::create("Empty response"));
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit requestFailed(QSharedPointer<UnknownError>::create(QString::fromUtf8(data)));
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
    QByteArray data = reply.property("responseData").toByteArray();
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
    if (!m_networkClient.getUserId()) {
        qWarning() << "User ID is not set in NetworkClient.";
        emit requestFailed(QSharedPointer<BaseError>(new UnknownError("User ID is not set")));
        return;
    }

    const QJsonDocument data(dto.toJson());

    utils::Validator validator;
    if (dto.email) {
        validator.field("email", dto.email->toStdString()).notBlank().validateEmail();
    }
    if (dto.firstName) {
        validator.field("first_name", dto.firstName->toStdString()).notBlank();
    }
    if (dto.lastName) {
        validator.field("last_name", dto.lastName->toStdString()).notBlank();
    }
    if (dto.password) {
        validator.field("password", dto.password->toStdString()).notBlank().validatePasswordStrength();
    }
    if (auto error = validator.getValidationError()) {
        emit requestFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
    m_networkClient.put(
        QUrl(QString("/user/%1").arg(*m_networkClient.getUserId())),
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
    utils::Validator validator;
    validator.field("email", dto.email.toStdString()).notBlank().validateEmail();
    validator.field("password", dto.password.toStdString()).notBlank().validatePasswordStrength();
    validator.field("first_name", dto.firstName.toStdString()).notBlank();
    validator.field("last_name", dto.lastName.toStdString()).notBlank();
    if (auto error = validator.getValidationError()) {
        emit requestFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
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
