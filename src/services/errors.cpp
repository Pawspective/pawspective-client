#include "services/errors.hpp"

namespace pawspective::services {

BaseError::BaseError(const QString& message) : m_message(message) {}

QString BaseError::getMessage() const { return m_message; }

ClientJsonParseError::ClientJsonParseError(const QString& message) : BaseError(message) {}

UnknownError::UnknownError(const QString& message) : BaseError(message) {}

}  // namespace pawspective::services
