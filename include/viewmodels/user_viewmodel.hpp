#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include "services/auth_service.hpp"
#include "viewmodels/base.hpp"
#include "services/user_service.hpp"
#include "models/user_dto.hpp"

namespace pawspective::viewmodels {

/**
 * @brief ViewModel для отображения данных проlфиля пользователя (только чтение)
 *
 * Предоставляет UI слой с данными о пользователе.
 * Следит за состоянием аутентификации и автоматически обновляет данные.
 */
class UserViewModel : public BaseViewModel {
    Q_OBJECT

    /**
     * @brief Сам объект с данными пользователя
     *
     * QML может обращаться к полям через: userData.email, userData.firstName и т.д.
     */
    Q_PROPERTY(models::UserDTO userData READ getUserData NOTIFY userDataChanged)

    /**
     * @brief Статус аутентификации
     */
    Q_PROPERTY(bool isAuthenticated READ getIsAuthenticated NOTIFY authStateChanged)

public:
    explicit UserViewModel(
        services::AuthService& authService,
        services::UserService& userService,
        QObject* parent = nullptr
    );
    ~UserViewModel() override = default;

    // Геттеры
    const models::UserDTO& getUserData() const { return m_userData; }
    bool getIsAuthenticated() const { return m_isAuthenticated; }

    /**
     * @brief Инициализация ViewModel
     *
     * Загружает данные текущего пользователя при старте
     */
    Q_INVOKABLE void initialize() override;

    /**
     * @brief Очистка ресурсов ViewModel
     */
    Q_INVOKABLE void cleanup() override;

    /**
     * @brief Обновить данные пользователя
     *
     * Принудительно запрашивает актуальные данные с сервера
     */
    Q_INVOKABLE void refreshUserData();

signals:
    /**
     * @brief Сигнал об изменении данных пользователя
     */
    void userDataChanged();

    /**
     * @brief Сигнал об изменении состояния аутентификации
     */
    void authStateChanged();

    /**
     * @brief Сигнал об успешной загрузке данных
     */
    void userDataLoaded();

    /**
     * @brief Сигнал об ошибке загрузки данных
     */
    void userDataLoadFailed(const QString& errorMessage);

private slots:
    // Обработчики сигналов от AuthService
    void handleLoginSuccess(
        const QString& accessToken,
        const QString& refreshToken,
        const QString& tokenType
    );
    void handleLoginFailed(QSharedPointer<services::BaseError> error);
    void handleLogoutSuccess();
    void handleLogoutFailed(QSharedPointer<services::BaseError> error);
    void handleGetCurrentUserSuccess(const models::UserDTO& user);
    void handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error);

    // Обработчики сигналов от UserService
    void handleRegisterSuccess(const models::UserDTO& user);
    void handleUpdateUserProfileSuccess(const models::UserDTO& user);

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