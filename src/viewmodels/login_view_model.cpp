#include "viewmodels/login_view_model.hpp"
#include "services/errors.hpp"
#include "utils/logger.hpp"

namespace pawspective::viewmodels {
LoginViewModel::LoginViewModel(services::AuthService& authService, QObject* parent)
    : BaseViewModel(parent), m_authService(authService) {
    DEBUG_LOG("LoginViewModel", "Constructor called");
    connect(&m_authService, &services::AuthService::loginSuccess, this, [this]() {
        DEBUG_LOG("LoginViewModel", "loginSuccess signal received");
        setIsBusy(false);
        emit loginFinished(true);
    });

    connect(
        &m_authService,
        &services::AuthService::loginFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            DEBUG_LOG("LoginViewModel", "loginFailed signal received, error:" << error->getMessage());
            setIsBusy(false);
            if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
                emitError(ValidationError, formatValidationError(validationError));
            } else {
                emitError(AuthenticationError, error->getMessage());
            }
            emit loginFinished(false);
        }
    );
}

void LoginViewModel::login() {
    DEBUG_LOG("LoginViewModel::login", "Called with email:" << m_email);
    if (m_email.isEmpty() || m_password.isEmpty()) {
        DEBUG_LOG("LoginViewModel::login", "Validation failed: empty email or password");
        emitError(ErrorType::ValidationError, "Email and password cannot be empty.");
        return;
    }
    DEBUG_LOG("LoginViewModel::login", "Starting login process");
    setIsBusy(true);
    m_authService.login(m_email, m_password);
}
}  // namespace pawspective::viewmodels