#pragma once
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <cstdint>
#include <functional>
#include <optional>
#include "models/user_dto.hpp"
#include "services/errors.hpp"
#include "services/network_client.hpp"
#include "state/auth_state.hpp"

namespace pawspective::services {

class AuthService : public QObject {
    Q_OBJECT
public:
    explicit AuthService(NetworkClient& networkClient, state::AuthState& authState, QObject* parent = nullptr);

    void login(const QString& email, const QString& password);
    void logout();
    void refreshToken(const QString& refreshToken);
    void getCurrentUser();

    bool isAuthenticated() const;
    bool hasRefreshToken() const;

    void restoreSession();

signals:
    void loginSuccess(
        const QString& accessToken,
        const QString& refreshToken,
        const QString& tokenType,
        uint64_t userId
    );
    void logoutSuccess();
    void sessionEnded();
    void sessionRestored();
    void refreshSuccess(const QString& accessToken, const QString& refreshToken, const QString& tokenType);
    void getCurrentUserSuccess(const models::UserDTO& user);

    void loginFailed(QSharedPointer<services::BaseError> error);
    void logoutFailed(QSharedPointer<services::BaseError> error);
    void refreshFailed(QSharedPointer<services::BaseError> error);
    void getCurrentUserFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    void handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError);
    void handleSuccess(
        QNetworkReply& reply,
        std::function<void(const QJsonObject&)> onSuccess,
        std::function<void(QSharedPointer<BaseError>)> onError
    );
    void handleUnauthorizedAccess();
    void clearSession();
    void clearSessionSilently();

    std::tuple<QString, QString, QString> parseTokenResponse(const QJsonObject& obj);

    NetworkClient& m_networkClient;
    state::AuthState& m_authState;
    QString m_accessToken;
    QString m_refreshToken;
    std::optional<std::uint64_t> m_userId;
    bool m_isRefreshing = false;
    bool m_isRestoringSession = false;
};

}  // namespace pawspective::services
