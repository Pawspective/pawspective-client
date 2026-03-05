#include "viewmodels/user_viewmodel.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

UserViewModel::UserViewModel(services::AuthService& authService, services::UserService& userService, QObject* parent)
    : BaseViewModel(parent), m_authService(authService), m_userService(userService), m_isAuthenticated(false) {
    // AuthService signals
    connect(&m_authService, &services::AuthService::loginSuccess, this, &UserViewModel::handleLoginSuccess);
    connect(&m_authService, &services::AuthService::loginFailed, this, &UserViewModel::handleLoginFailed);
    connect(&m_authService, &services::AuthService::logoutSuccess, this, &UserViewModel::handleLogoutSuccess);
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

const models::UserDTO& UserViewModel::getUserData() const { return m_userData; }
bool UserViewModel::getIsAuthenticated() const { return m_isAuthenticated; }

void UserViewModel::initialize() {
    if (m_authService.isAuthenticated()) {
        m_isAuthenticated = true;
        emit authStateChanged();
        loadUserData();
    } else {
        m_isAuthenticated = false;
        emit authStateChanged();
    }
}

void UserViewModel::cleanup() {}

void UserViewModel::refreshUserData() {
    if (m_isAuthenticated && !isBusy()) {
        loadUserData();
    }
}

void UserViewModel::loadUserData() {
    if (isBusy()) {
        return;
    }
    setIsBusy(true);
    m_authService.getCurrentUser();
}

// AuthService handlers
void UserViewModel::handleLoginSuccess(const QString&, const QString&, const QString&) {
    m_isAuthenticated = true;
    emit authStateChanged();
    loadUserData();
}

void UserViewModel::handleLoginFailed(QSharedPointer<services::BaseError> error) {
    QString errorMsg = errorToString(error);
    m_isAuthenticated = false;
    emit authStateChanged();
    emitError(ErrorType::AuthenticationError, errorMsg);
}

void UserViewModel::handleLogoutSuccess() {
    m_isAuthenticated = false;
    clearUserData();
    emit authStateChanged();
}

void UserViewModel::handleLogoutFailed(QSharedPointer<services::BaseError> error) {
    QString errorMsg = errorToString(error);
    m_isAuthenticated = false;
    clearUserData();
    emit authStateChanged();
    emitError(ErrorType::NetworkError, errorMsg);
}

void UserViewModel::handleGetCurrentUserSuccess(const models::UserDTO& user) {
    setIsBusy(false);
    m_isAuthenticated = true;
    updateUserData(user);
    emit userDataLoaded();
}

void UserViewModel::handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    QString errorMsg = errorToString(error);
    emitError(ErrorType::NetworkError, errorMsg);
    emit userDataLoadFailed(errorMsg);
}

// UserService handlers
void UserViewModel::handleRegisterSuccess(const models::UserDTO& user) {
    m_isAuthenticated = true;
    emit authStateChanged();

    updateUserData(user);
    emit userDataLoaded();
}

void UserViewModel::handleUpdateUserProfileSuccess(const models::UserDTO& user) { updateUserData(user); }

void UserViewModel::handleTokenRefreshFailed(QSharedPointer<services::BaseError> error) {
    m_isAuthenticated = false;
    clearUserData();
    emit authStateChanged();
    if (error) {
        emitError(error->getType(), error->getMessage());
    }
    emit sessionExpired();
}

void UserViewModel::updateUserData(const models::UserDTO& user) {
    bool changed = false;

    if (m_userData.id != user.id) {
        m_userData.id = user.id;
        changed = true;
    }
    if (m_userData.email != user.email) {
        m_userData.email = user.email;
        changed = true;
    }
    if (m_userData.firstName != user.firstName) {
        m_userData.firstName = user.firstName;
        changed = true;
    }
    if (m_userData.lastName != user.lastName) {
        m_userData.lastName = user.lastName;
        changed = true;
    }
    if (m_userData.organizationId != user.organizationId) {
        m_userData.organizationId = user.organizationId;
        changed = true;
    }

    if (changed) {
        emit userDataChanged();
    }
}

void UserViewModel::clearUserData() {
    m_userData = models::UserDTO();
    emit userDataChanged();
}

QString UserViewModel::errorToString(QSharedPointer<services::BaseError> error) const {
    if (error.isNull()) {
        return tr("Unknown error");
    }
    return error->getMessage();
}

}  // namespace pawspective::viewmodels