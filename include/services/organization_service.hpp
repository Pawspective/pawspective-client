#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

#include "models/organization_dto.hpp"
#include "models/organization_register_dto.hpp"
#include "models/organization_update_dto.hpp"
#include "services/errors.hpp"
#include "services/network_client.hpp"

namespace pawspective::services {

class OrganizationService : public QObject {
    Q_OBJECT
public:
    explicit OrganizationService(NetworkClient& networkClient, QObject* parent = nullptr);

    void getOrganization(qint64 id);
    void createOrganization(const models::OrganizationRegisterDTO& dto);
    void updateOrganization(qint64 id, const models::OrganizationUpdateDTO& dto);

signals:
    void getOrganizationSuccess(const models::OrganizationDTO& organization);
    void createOrganizationSuccess(const models::OrganizationDTO& organization);
    void updateOrganizationSuccess(const models::OrganizationDTO& organization);
    void requestFailed(QSharedPointer<services::BaseError> error);

private:
    void handleError(QNetworkReply& reply);
    void handleSuccess(QNetworkReply& reply, std::function<void(const QJsonObject&)> onSuccess);

    NetworkClient& m_networkClient;
};

}  // namespace pawspective::services
