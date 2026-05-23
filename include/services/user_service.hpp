#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

#include "models/user_dto.hpp"
#include "models/user_register_dto.hpp"
#include "models/user_update_dto.hpp"
#include "services/errors.hpp"
#include "services/network_client.hpp"

namespace pawspective::services {
class UserService : public QObject {
    Q_OBJECT
public:
    explicit UserService(NetworkClient& networkClient, QObject* parent = nullptr);

    void updateUserProfile(const models::UserUpdateDTO& dto);
    void registerUser(const models::UserRegisterDTO& dto);
    void deleteUser();

signals:
    void updateUserProfileSuccess(const models::UserDTO& user);
    void registerUserSuccess(const models::UserDTO& user);
    void deleteUserSuccess();
    void deleteUserFailed(QSharedPointer<services::BaseError> error);
    void requestFailed(QSharedPointer<services::BaseError> error);

private:
    void handleError(QNetworkReply& reply);
    void handleSuccess(QNetworkReply& reply, std::function<void(const QJsonObject&)> onSuccess);

    NetworkClient& m_networkClient;
};
}  // namespace pawspective::services
