#include "viewmodels/user_viewmodel.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

UserViewModel::UserViewModel(services::AuthService& authService, services::UserService& userService, QObject* parent)
    : BaseViewModel(parent), m_authService(authService), m_userService(userService), m_isAuthenticated(false) {
    // AuthService signals
    connect(&m_authService, &services::AuthService::loginSuccess, this, &UserViewModel::handleLoginSuccess);
    connect(&m_authService, &services::AuthService::loginFailed, this, &UserViewModel::handleLoginFailed);
    connect(&m_authService, &services::AuthService::sessionEnded, this, &UserViewModel::handleLogoutSuccess);
    connect(&m_authService, &services::AuthService::logoutFailed, this, &UserViewModel::handleLogoutFailed);
    connect(
        &m_authService,
        &services::AuthService::getCurrentUserSuccess,
        this,
        &UserViewModel::handleGetCurrentUserSuccess
    );
    connect(
        &m_authService,
        &services::AuthService::getCurrentUserFailed,
        this,
        &UserViewModel::handleGetCurrentUserFailed
    );
    // UserService signals
    connect(&m_userService, &services::UserService::registerUserSuccess, this, &UserViewModel::handleRegisterSuccess);
    connect(
        &m_userService,
        &services::UserService::updateUserProfileSuccess,
        this,
        &UserViewModel::handleUpdateUserProfileSuccess
    );
    connect(&m_authService, &services::AuthService::refreshFailed, this, &UserViewModel::handleTokenRefreshFailed);
}

const models::UserDTO& UserViewModel::userData() const { return m_userData; }
bool UserViewModel::isAuthenticated() const { return m_isAuthenticated; }

void UserViewModel::initialize() {
    updateProperty(m_isAuthenticated, m_authService.isAuthenticated(), [this] { emit authStateChanged(); });
    if (m_isAuthenticated) {
        loadUserData();
    }
}

void UserViewModel::logout() {
    if (isBusy()) {
        return;
    }
    setIsBusy(true);
    m_authService.logout();
}

void UserViewModel::cleanup() { clearUserData(); }

void UserViewModel::refreshUserData() { loadUserData(); }

void UserViewModel::loadUserData() {
    if (isBusy()) {
        return;
    }
    setIsBusy(true);
    m_authService.getCurrentUser();
}

// AuthService handlers
void UserViewModel::handleLoginSuccess(const QString&, const QString&, const QString&, uint64_t /*userId*/) {
    updateProperty(m_isAuthenticated, true, [this] { emit authStateChanged(); });
    loadUserData();
}

void UserViewModel::handleLoginFailed(QSharedPointer<services::BaseError> error) {
    updateProperty(m_isAuthenticated, false, [this] { emit authStateChanged(); });
    if (error) {
        emitError(ErrorType::AuthenticationError, error->getMessage());
    }
}

void UserViewModel::handleLogoutSuccess() {
    updateProperty(m_isAuthenticated, false, [this] { emit authStateChanged(); });
    clearUserData();
}

void UserViewModel::handleLogoutFailed(QSharedPointer<services::BaseError> error) {
    if (error) {
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void UserViewModel::handleGetCurrentUserSuccess(const models::UserDTO& user) {
    setIsBusy(false);
    updateUserData(user);
    updateProperty(m_isAuthenticated, true, [this] { emit authStateChanged(); });
    emit userDataLoaded();
}

void UserViewModel::handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    updateProperty(m_isAuthenticated, false, [this] { emit authStateChanged(); });
    if (error) {
        emit userDataLoadFailed(error->getMessage());
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

// UserService handlers
void UserViewModel::handleRegisterSuccess(const models::UserDTO& user) { updateUserData(user); }

void UserViewModel::handleUpdateUserProfileSuccess(const models::UserDTO& user) { updateUserData(user); }

void UserViewModel::handleTokenRefreshFailed(QSharedPointer<services::BaseError> error) {
    updateProperty(m_isAuthenticated, false, [this] { emit authStateChanged(); });
    clearUserData();
    if (error) {
        emitError(ErrorType::AuthenticationError, error->getMessage());
    }
    emit sessionExpired();
}

void UserViewModel::updateUserData(const models::UserDTO& user) {
    updateProperty(m_userData, user, [this] { emit userDataChanged(); });
}

void UserViewModel::clearUserData() {
    updateProperty(m_userData, models::UserDTO{}, [this] { emit userDataChanged(); });
}

}  // namespace pawspective::viewmodels