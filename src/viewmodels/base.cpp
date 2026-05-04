#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {
BaseViewModel::BaseViewModel(QObject* parent) : QObject(parent) {}

bool BaseViewModel::isBusy() const { return m_isBusy; }

void BaseViewModel::setIsBusy(bool value) {
    updateProperty(m_isBusy, value, [this]() { emit isBusyChanged(); });
}

void BaseViewModel::emitError(ErrorType type, const QString& message) { emit errorOccurred(type, message); }

QString BaseViewModel::formatValidationError(QSharedPointer<services::BaseError> error) {
    if (!error) {
        return "Unknown error";
    }

    if (auto* validationError = dynamic_cast<const services::ValidationError*>(error.data())) {
        QStringList messages;
        for (const auto& fieldError : validationError->getErrors()) {
            messages
                << QString("%1: %2")
                       .arg(QString::fromStdString(fieldError.fieldName))
                       .arg(QString::fromStdString(fieldError.errorMessage));
        }
        return messages.isEmpty() ? error->getMessage() : messages.join("\n");
    }

    return error->getMessage();
}

}  // namespace pawspective::viewmodels
