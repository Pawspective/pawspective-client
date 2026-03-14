#include "viewmodels/user_update_viewmodel.hpp"

namespace pawspective::viewmodels {

UserUpdateViewModel::UserUpdateViewModel(
    services::UserService& userService,
    services::AuthService& authService,
    QObject* parent
)
    : BaseViewModel(parent), m_userService(userService), m_authService(authService), m_isDirty(false) {
    connect(
        &m_userService,
        &services::UserService::updateUserProfileSuccess,
        this,
        &UserUpdateViewModel::handleUpdateSuccess
    );
    connect(&m_userService, &services::UserService::requestFailed, this, &UserUpdateViewModel::handleRequestFailed);
    connect(
        &m_authService,
        &services::AuthService::getCurrentUserSuccess,
        this,
        &UserUpdateViewModel::handleGetCurrentUserSuccess
    );
    connect(
        &m_authService,
        &services::AuthService::refreshFailed,
        this,
        &UserUpdateViewModel::handleTokenRefreshFailed
    );
}

void UserUpdateViewModel::initialize() {
    if (!isBusy()) {
        setIsBusy(true);
        m_authService.getCurrentUser();
    }
}

void UserUpdateViewModel::cleanup() {
    setIsBusy(false);
    discardChanges();
}

// Геттеры
QString UserUpdateViewModel::email() const {
    if (m_changes.email.has_value()) {
        return m_changes.email.value();
    }
    return m_originalData.email;
}

QString UserUpdateViewModel::firstName() const {
    if (m_changes.firstName.has_value()) {
        return m_changes.firstName.value();
    }
    return m_originalData.firstName;
}

QString UserUpdateViewModel::lastName() const {
    if (m_changes.lastName.has_value()) {
        return m_changes.lastName.value();
    }
    return m_originalData.lastName;
}

QString UserUpdateViewModel::newPassword() const {
    if (m_changes.password.has_value()) {
        return m_changes.password.value();
    }
    return QString();
}

// Сеттеры
void UserUpdateViewModel::setEmail(const QString& value) {
    if (email() == value) {
        return;
    }

    m_changes.email = value;
    emit emailChanged();
    setDirty(true);
}

void UserUpdateViewModel::setFirstName(const QString& value) {
    if (firstName() == value) {
        return;
    }

    m_changes.firstName = value;
    emit firstNameChanged();
    setDirty(true);
}

void UserUpdateViewModel::setLastName(const QString& value) {
    if (lastName() == value) {
        return;
    }

    m_changes.lastName = value;
    emit lastNameChanged();
    setDirty(true);
}

void UserUpdateViewModel::setNewPassword(const QString& password) {
    if (newPassword() == password) {
        return;
    }

    if (password.isEmpty()) {
        m_changes.password.reset();
    } else {
        m_changes.password = password;
    }
    emit newPasswordChanged();
    setDirty(true);
}

void UserUpdateViewModel::saveChanges() {
    if (!m_isDirty || isBusy()) {
        return;
    }

    setIsBusy(true);
    emit saveStarted();
    m_userService.updateUserProfile(m_changes);
}

void UserUpdateViewModel::discardChanges() {
    m_changes = models::UserUpdateDTO();
    setDirty(false);
    notifyAllChanged();
}

void UserUpdateViewModel::setDirty(bool dirty) {
    updateProperty(m_isDirty, dirty, [this] { emit dirtyChanged(); });
}

void UserUpdateViewModel::handleGetCurrentUserSuccess(const models::UserDTO& user) {
    setIsBusy(false);
    m_originalData = user;
    m_changes = models::UserUpdateDTO();

    setDirty(false);
    notifyAllChanged();
}

void UserUpdateViewModel::handleUpdateSuccess(const models::UserDTO& user) {
    setIsBusy(false);

    m_originalData = user;
    m_changes = models::UserUpdateDTO();

    setDirty(false);
    notifyAllChanged();

    emit saveCompleted(user);
}

void UserUpdateViewModel::handleRequestFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);

    QString errorMessage = error ? error->getMessage() : "Unknown error";
    ;
    emit saveFailed(errorMessage);
    emitError(ErrorType::NetworkError, errorMessage);
}

void UserUpdateViewModel::handleTokenRefreshFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (error) {
        emit saveFailed(error->getMessage());
        emitError(ErrorType::AuthenticationError, error->getMessage());
    }
    emit sessionExpired();
}

void UserUpdateViewModel::notifyAllChanged() {
    emit emailChanged();
    emit firstNameChanged();
    emit lastNameChanged();
    emit newPasswordChanged();
}

}  // namespace pawspective::viewmodels