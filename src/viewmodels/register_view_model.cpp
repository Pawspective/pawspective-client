#include "viewmodels/register_view_model.hpp"
#include "models/user_register_dto.hpp"
#include "services/errors.hpp"
#include "validator.hpp"

namespace pawspective::viewmodels {
RegisterViewModel::RegisterViewModel(services::UserService& userService, QObject* parent)
    : BaseViewModel(parent), m_userService(userService) {
    connect(&m_userService, &services::UserService::registerUserSuccess, this, [this]() {
        setIsBusy(false);
        emit registrationFinished(true);
    });

    connect(
        &m_userService,
        &services::UserService::requestFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            setIsBusy(false);
            emitError(ValidationError, error->getMessage());
            emit registrationFinished(false);
        }
    );
}

void RegisterViewModel::registerUser() {
    if (m_firstName.isEmpty() || m_lastName.isEmpty() || m_email.isEmpty() || m_password.isEmpty() ||
        m_confirmPassword.isEmpty())
    {
        emitError(ErrorType::ValidationError, "All fields are required.");
        return;
    }

    if (m_password != m_confirmPassword) {
        emitError(ErrorType::ValidationError, "Passwords do not match.");
        return;
    }
    if (!pawspective::utils::validation::validateEmail(m_email.toStdString())) {
        emitError(ErrorType::ValidationError, "Invalid email format.");
        return;
    }
    pawspective::models::UserRegisterDTO dto{m_email, m_password, m_firstName, m_lastName};
    setIsBusy(true);

    m_userService.registerUser(dto);
}
}  // namespace pawspective::viewmodels