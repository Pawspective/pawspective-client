#include "services/errors.hpp"
#include <qjsondocument.h>

namespace pawspective::services {

namespace {
ErrorType parseErrorType(const QString& code) {
    static const std::unordered_map<QString, ErrorType> ErrorMap = {
        {"VALIDATION_ERROR", ErrorType::ValidationErrorType},
        {"ACCESS_TOKEN_EXPIRED", ErrorType::AccessTokenExpiredErrorType},
        {"ACCESS_TOKEN_INVALID", ErrorType::AccessTokenInvalidErrorType},
        {"INVALID_CREDENTIALS", ErrorType::InvalidCredentialsErrorType}
    };
    return ErrorMap.contains(code) ? ErrorMap.at(code) : ErrorType::UnknownErrorType;
}

[[nodiscard]] QString extractMessage(const QJsonObject& errorResponse, const QString& defaultMessage) {
    if (errorResponse.contains("message") && errorResponse["message"].isString()) {
        return errorResponse["message"].toString();
    }
    return defaultMessage;
}
}  // namespace

BaseError::BaseError(const QString& message) : m_message(message) {}

QString BaseError::getMessage() const { return m_message; }

ValidationError::ValidationError(const QString& message) : BaseError(message) {}

ClientJsonParseError::ClientJsonParseError(const QString& message) : BaseError(message) {}

AccessTokenExpiredError::AccessTokenExpiredError(const QJsonObject& errorResponse)
    : BaseError(extractMessage(errorResponse, "Access token expired")) {}

AccessTokenInvalidError::AccessTokenInvalidError(const QJsonObject& errorResponse)
    : BaseError(extractMessage(errorResponse, "Access token invalid")) {}

InvalidCredentialsError::InvalidCredentialsError(const QJsonObject& errorResponse)
    : BaseError(extractMessage(errorResponse, "Invalid credentials")) {}

UnknownError::UnknownError(const QString& message) : BaseError(message) {}

QSharedPointer<BaseError> ErrorFactory::createError(const QByteArray& data) {
    QJsonParseError parseError;
    try {
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            return QSharedPointer<BaseError>(new ClientJsonParseError(
                QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
            ));
        }
        return createError(doc.object());
    } catch (const std::exception& e) {
        return QSharedPointer<BaseError>(new ClientJsonParseError(QString(e.what())));
    }
}

QSharedPointer<BaseError> ErrorFactory::createError(const QJsonObject& errorResponse) {
    if (!errorResponse.contains("error") || !errorResponse["error"].isObject()) {
        return QSharedPointer<UnknownError>::create("Invalid error response format");
    }
    QJsonObject errorObject = errorResponse["error"].toObject();
    ErrorType errorCode{};

    if (errorObject.contains("code") && errorObject["code"].isString()) {
        errorCode = parseErrorType(errorObject["code"].toString());
    }
    switch (errorCode) {
        case ErrorType::AccessTokenExpiredErrorType:
            return QSharedPointer<AccessTokenExpiredError>::create(errorObject);
        case ErrorType::AccessTokenInvalidErrorType:
            return QSharedPointer<AccessTokenInvalidError>::create(errorObject);
        case ErrorType::InvalidCredentialsErrorType:
            return QSharedPointer<InvalidCredentialsError>::create(errorObject);
        default:
            return QSharedPointer<UnknownError>::create("Unknown code type in error response");
    }
}

}  // namespace pawspective::services
