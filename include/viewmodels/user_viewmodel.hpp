#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include "models/user_dto.hpp"
#include "services/auth_service.hpp"
#include "services/user_service.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

class UserViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(models::UserDTO userData READ getUserData NOTIFY userDataChanged)

    Q_PROPERTY(bool isAuthenticated READ getIsAuthenticated NOTIFY authStateChanged)

public:
    explicit UserViewModel(
        services::AuthService& authService,
        services::UserService& userService,
        QObject* parent = nullptr
    );
    ~UserViewModel() override = default;

    const models::UserDTO& getUserData() const;
    bool getIsAuthenticated() const;

    Q_INVOKABLE void initialize() override;

    Q_INVOKABLE void cleanup() override;

    Q_INVOKABLE void refreshUserData();

signals:

    void userDataChanged();
    void authStateChanged();
    void userDataLoaded();
    void userDataLoadFailed(const QString& errorMessage);
    void sessionExpired();

private slots:
    void handleLoginSuccess(const QString& accessToken, const QString& refreshToken, const QString& tokenType);
    void handleLoginFailed(QSharedPointer<services::BaseError> error);
    void handleLogoutSuccess();
    void handleLogoutFailed(QSharedPointer<services::BaseError> error);
    void handleGetCurrentUserSuccess(const models::UserDTO& user);
    void handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error);

    void handleRegisterSuccess(const models::UserDTO& user);
    void handleUpdateUserProfileSuccess(const models::UserDTO& user);
    void handleTokenRefreshFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    services::AuthService& m_authService;
    services::UserService& m_userService;
    models::UserDTO m_userData;
    bool m_isAuthenticated;

    void loadUserData();
    void updateUserData(const models::UserDTO& user);
    void clearUserData();
    QString errorToString(QSharedPointer<services::BaseError> error) const;
};

}  // namespace pawspective::viewmodels