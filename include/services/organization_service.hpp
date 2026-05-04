#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

#include "models/organization_dto.hpp"
#include "models/organization_register_dto.hpp"
#include "models/organization_update_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

class OrganizationService : public QObject {
    Q_OBJECT
public:
    explicit OrganizationService(INetworkClient& networkClient, QObject* parent = nullptr);

    void getOrganization(qint64 id);
    void createOrganization(const models::OrganizationRegisterDTO& dto);
    void updateOrganization(qint64 id, const models::OrganizationUpdateDTO& dto);
    void findByNameContaining(const QString& name, int page = 1);

signals:
    void getOrganizationSuccess(const models::OrganizationDTO& organization);
    void createOrganizationSuccess(const models::OrganizationDTO& organization);
    void updateOrganizationSuccess(const models::OrganizationDTO& organization);
    void findByNameContainingSuccess(const models::OrganizationListDTO& result);

    void getOrganizationFailed(QSharedPointer<services::BaseError> error);
    void createOrganizationFailed(QSharedPointer<services::BaseError> error);
    void updateOrganizationFailed(QSharedPointer<services::BaseError> error);
    void findByNameContainingFailed(QSharedPointer<services::BaseError> error);

private:
    void handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError);
    void handleSuccess(
        QNetworkReply& reply,
        std::function<void(const QJsonObject&)> onSuccess,
        std::function<void(QSharedPointer<BaseError>)> onError
    );
    void handleSuccessArray(
        QNetworkReply& reply,
        std::function<void(const QJsonArray&)> onSuccess,
        std::function<void(QSharedPointer<BaseError>)> onError
    );

    INetworkClient& m_networkClient;
};

}  // namespace pawspective::services
