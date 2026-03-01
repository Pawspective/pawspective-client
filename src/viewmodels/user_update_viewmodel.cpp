#include "viewmodels/user_update_viewmodel.hpp"
#include <QDebug>

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
QString UserUpdateViewModel::getEmail() const {
    if (m_changes.email.has_value()) {
        return m_changes.email.value();
    }
    return m_originalData.email;
}

QString UserUpdateViewModel::getFirstName() const {
    if (m_changes.firstName.has_value()) {
        return m_changes.firstName.value();
    }
    return m_originalData.firstName;
}

QString UserUpdateViewModel::getLastName() const {
    if (m_changes.lastName.has_value()) {
        return m_changes.lastName.value();
    }
    return m_originalData.lastName;
}

QString UserUpdateViewModel::getNewPassword() const {
    if (m_changes.password.has_value()) {
        return m_changes.password.value();
    }
    return QString();
}

bool UserUpdateViewModel::getIsDirty() const { return m_isDirty; }

// Сеттеры
void UserUpdateViewModel::setEmail(const QString& email) {
    if (getEmail() == email) {
        return;
    }

    m_changes.email = email;
    emit emailChanged();
    updateDirtyState();
}

void UserUpdateViewModel::setFirstName(const QString& firstName) {
    if (getFirstName() == firstName) {
        return;
    }

    m_changes.firstName = firstName;
    emit firstNameChanged();
    updateDirtyState();
}

void UserUpdateViewModel::setLastName(const QString& lastName) {
    if (getLastName() == lastName) {
        return;
    }

    m_changes.lastName = lastName;
    emit lastNameChanged();
    updateDirtyState();
}

void UserUpdateViewModel::setNewPassword(const QString& password) {
    if (getNewPassword() == password) {
        return;
    }

    if (password.isEmpty()) {
        m_changes.password.reset();
    } else {
        m_changes.password = password;
    }
    emit newPasswordChanged();
    updateDirtyState();
}

void UserUpdateViewModel::saveChanges() {
    if (isBusy()) {
        return;
    }

    setIsBusy(true);
    emit saveStarted();
    m_userService.updateUserProfile(m_changes);
}

void UserUpdateViewModel::discardChanges() {
    m_changes = models::UserUpdateDTO();

    emit emailChanged();
    emit firstNameChanged();
    emit lastNameChanged();
    emit newPasswordChanged();

    m_isDirty = false;
    emit dirtyChanged();
}

void UserUpdateViewModel::updateDirtyState() {
    bool dirty =
        m_changes.email.has_value() || m_changes.firstName.has_value() || m_changes.lastName.has_value() ||
        m_changes.password.has_value();

    if (m_isDirty != dirty) {
        m_isDirty = dirty;
        emit dirtyChanged();
    }
}

void UserUpdateViewModel::handleGetCurrentUserSuccess(const models::UserDTO& user) {
    setIsBusy(false);
    m_originalData = user;
    m_changes = models::UserUpdateDTO();

    emit emailChanged();
    emit firstNameChanged();
    emit lastNameChanged();
    emit newPasswordChanged();

    m_isDirty = false;
    emit dirtyChanged();
}

void UserUpdateViewModel::handleUpdateSuccess(const models::UserDTO& user) {
    setIsBusy(false);

    m_originalData = user;
    m_changes = models::UserUpdateDTO();

    emit emailChanged();
    emit firstNameChanged();
    emit lastNameChanged();
    emit newPasswordChanged();

    m_isDirty = false;
    emit dirtyChanged();

    emit saveCompleted(user);
}

void UserUpdateViewModel::handleRequestFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);

    QString errorMessage = errorToString(error);
    emit saveFailed(errorMessage);
    emitError(ErrorType::NetworkError, errorMessage);
}

void UserUpdateViewModel::handleTokenRefreshFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (error) {
        emit saveFailed(error->getMessage());
        emitError(error->getType(), error->getMessage());
    }
    emit sessionExpired();
}

QString UserUpdateViewModel::errorToString(QSharedPointer<services::BaseError> error) const {
    return error.isNull() ? tr("Unknown error") : error->getMessage();
}

}  // namespace pawspective::viewmodels