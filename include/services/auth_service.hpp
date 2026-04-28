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

namespace pawspective::services {

class AuthService : public QObject {
    Q_OBJECT
public:
    explicit AuthService(NetworkClient& networkClient, QObject* parent = nullptr);

    // Authentication methods
    void login(const QString& email, const QString& password);
    void logout();
    void refreshToken(const QString& refreshToken);
    void getCurrentUser();

    // Token management
    bool isAuthenticated() const;

signals:
    void loginSuccess(
        const QString& accessToken,
        const QString& refreshToken,
        const QString& tokenType,
        uint64_t userId
    );
    void sessionEnded();
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

    std::tuple<QString, QString, QString> parseTokenResponse(const QJsonObject& obj);

    NetworkClient& m_networkClient;
    QString m_accessToken;
    QString m_refreshToken;
    std::optional<std::uint64_t> m_userId;
    bool m_isRefreshing = false;
};

}  // namespace pawspective::services