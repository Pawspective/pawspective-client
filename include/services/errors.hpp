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
    InvalidCredentialsErrorType,
    UserNotFoundErrorType,
    UserAlreadyExistsErrorType,
    OrganizationNotFoundErrorType,
    CityNotFoundErrorType,
    BreedNotFoundErrorType,
    AnimalNotFoundErrorType,
    ForbiddenErrorType,
    RefreshTokenInvalidErrorType,
    MissingFieldErrorType,
    InvalidJsonFormatErrorType
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
    struct FieldError {
        std::string fieldName;
        std::string errorMessage;
    };

    explicit ValidationError(const std::vector<FieldError>& errors, std::string msg = "Validation error.");
    explicit ValidationError(const QJsonObject& errorResponse);

    const std::vector<FieldError>& getErrors() const;

private:
    std::vector<FieldError> m_errors;
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

class UserNotFoundError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::UserNotFoundErrorType;
    explicit UserNotFoundError(const QString& message) : BaseError(message) {}
};

class OrganizationNotFoundError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::OrganizationNotFoundErrorType;
    explicit OrganizationNotFoundError(const QString& message) : BaseError(message) {}
};

class CityNotFoundError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::CityNotFoundErrorType;
    explicit CityNotFoundError(const QString& message) : BaseError(message) {}
};

class BreedNotFoundError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::BreedNotFoundErrorType;
    explicit BreedNotFoundError(const QString& message) : BaseError(message) {}
};

class AnimalNotFoundError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::AnimalNotFoundErrorType;
    explicit AnimalNotFoundError(const QString& message) : BaseError(message) {}
};

class UserAlreadyExistsError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::UserAlreadyExistsErrorType;
    explicit UserAlreadyExistsError(const QString& message) : BaseError(message) {}
};

class ForbiddenError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::ForbiddenErrorType;
    explicit ForbiddenError(const QString& message) : BaseError(message) {}
};

class RefreshTokenInvalidError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::RefreshTokenInvalidErrorType;
    explicit RefreshTokenInvalidError(const QString& message) : BaseError(message) {}
};

class MissingFieldError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::MissingFieldErrorType;
    explicit MissingFieldError(const QString& message) : BaseError(message) {}
};

class InvalidJsonFormatError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::InvalidJsonFormatErrorType;
    explicit InvalidJsonFormatError(const QString& message) : BaseError(message) {}
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
