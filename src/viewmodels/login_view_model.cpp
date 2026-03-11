#include "viewmodels/login_view_model.hpp"
#include "services/errors.hpp"

namespace pawspective::viewmodels {
LoginViewModel::LoginViewModel(services::AuthService& authService, QObject* parent)
    : BaseViewModel(parent), m_authService(authService) {
    connect(&m_authService, &services::AuthService::loginSuccess, this, [this]() {
        setIsBusy(false);
        emit loginFinished(true);
    });

    connect(
        &m_authService,
        &services::AuthService::loginFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            setIsBusy(false);
            emitError(AuthenticationError, error->getMessage());
            emit loginFinished(false);
        }
    );
}

void LoginViewModel::login() {
    if (m_email.isEmpty() || m_password.isEmpty()) {
        emitError(ErrorType::ValidationError, "Email and password cannot be empty.");
        return;
    }
    setIsBusy(true);
    m_authService.login(m_email, m_password);
}
}  // namespace pawspective::viewmodels