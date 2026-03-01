#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include "models/user_dto.hpp"
#include "models/user_update_dto.hpp"
#include "services/auth_service.hpp"
#include "services/user_service.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

class UserUpdateViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(QString email READ getEmail WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString firstName READ getFirstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ getLastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(QString newPassword READ getNewPassword WRITE setNewPassword NOTIFY newPasswordChanged)
    Q_PROPERTY(bool isDirty READ getIsDirty NOTIFY dirtyChanged)

public:
    explicit UserUpdateViewModel(
        services::UserService& userService,
        services::AuthService& authService,
        QObject* parent = nullptr
    );
    ~UserUpdateViewModel() override = default;

    QString getEmail() const;
    QString getFirstName() const;
    QString getLastName() const;
    QString getNewPassword() const;
    bool getIsDirty() const;

    void setEmail(const QString& email);
    void setFirstName(const QString& firstName);
    void setLastName(const QString& lastName);
    void setNewPassword(const QString& password);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void discardChanges();

signals:
    void emailChanged();
    void firstNameChanged();
    void lastNameChanged();
    void newPasswordChanged();
    void dirtyChanged();
    void saveCompleted(const models::UserDTO& updatedUser);
    void saveFailed(const QString& errorMessage);
    void saveStarted();
    void sessionExpired();

private slots:
    void handleUpdateSuccess(const models::UserDTO& user);
    void handleRequestFailed(QSharedPointer<services::BaseError> error);
    void handleGetCurrentUserSuccess(const models::UserDTO& user);
    void handleTokenRefreshFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    services::UserService& m_userService;
    services::AuthService& m_authService;

    models::UserUpdateDTO m_changes;
    models::UserDTO m_originalData;

    bool m_isDirty;

    void updateDirtyState();
    QString errorToString(QSharedPointer<services::BaseError> error) const;
};

}  // namespace pawspective::viewmodels