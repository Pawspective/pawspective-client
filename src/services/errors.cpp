#include "services/errors.hpp"

namespace pawspective::services {

namespace {
ErrorType parseErrorType(const QString& code) {
    if (code == "VALIDATION_ERROR") {
        return ErrorType::ValidationErrorType;
    }
    return ErrorType::UnknownErrorType;
}
}  // namespace

BaseError::BaseError(const QString& message) : m_message(message) {}

QString BaseError::getMessage() const { return m_message; }

ValidationError::ValidationError(const QString& message) : BaseError(message) {}

ClientJsonParseError::ClientJsonParseError(const QString& message) : BaseError(message) {}

UnknownError::UnknownError(const QString& message) : BaseError(message) {}

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
        default:
            return QSharedPointer<UnknownError>::create("Unknown code type in error response");
    }
}

}  // namespace pawspective::services
