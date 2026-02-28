#pragma once

#include <QJsonObject>
#include <QString>

namespace pawspective::services {

enum ErrorType : uint8_t {
    ValidationErrorType,
    ClientJsonParseErrorType,
    UnknownErrorType,
};

class BaseError {
public:
    explicit BaseError(const QString& message);
    virtual QString getMessage() const;
    virtual ~BaseError() = default;

protected:
    QString m_message;  // NOLINT(misc-non-private-member-variables-in-classes)
};

class ClientJsonParseError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::ClientJsonParseErrorType;
    explicit ClientJsonParseError(const QString& message);
};

class UnknownError : public BaseError {
public:
    static constexpr ErrorType code = ErrorType::UnknownErrorType;
    explicit UnknownError(const QString& message);
};

class ErrorFactory {
public:
    static std::unique_ptr<BaseError> createError(const QJsonObject& errorResponse);
};

}  // namespace pawspective::services
