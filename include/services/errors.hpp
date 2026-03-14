#pragma once

#include <QJsonObject>
#include <QSharedPointer>
#include <QString>

namespace pawspective::services {

enum ErrorType : uint8_t {
    ValidationErrorType,
    ClientJsonParseErrorType,
    UnknownErrorType,
    AccessTokenExpiredErrorType,
    AccessTokenInvalidErrorType,
    InvalidCredentialsErrorType
};

class BaseError {
public:
    virtual QString getMessage() const;
    virtual ~BaseError() = default;

protected:
    explicit BaseError(const QString& message);
    QString m_message;  // NOLINT(misc-non-private-member-variables-in-classes)
};

class ValidationError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::ValidationErrorType;
    explicit ValidationError(const QString& message);
};

class ClientJsonParseError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::ClientJsonParseErrorType;
    explicit ClientJsonParseError(const QString& message);
};

class AccessTokenExpiredError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::AccessTokenExpiredErrorType;
    explicit AccessTokenExpiredError(const QJsonObject& errorResponse);
};

class AccessTokenInvalidError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::AccessTokenInvalidErrorType;
    explicit AccessTokenInvalidError(const QJsonObject& errorResponse);
};

class InvalidCredentialsError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::InvalidCredentialsErrorType;
    explicit InvalidCredentialsError(const QJsonObject& errorResponse);
};

class UnknownError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::UnknownErrorType;
    explicit UnknownError(const QString& message);
};

class ErrorFactory {
public:
    static QSharedPointer<BaseError> createError(const QByteArray& data);
    static QSharedPointer<BaseError> createError(const QJsonObject& errorResponse);
};

}  // namespace pawspective::services
