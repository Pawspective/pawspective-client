#pragma once

#include <QJsonObject>
#include <QObject>

#include "models/adopt_request_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

class AdoptRequestService : public QObject {
    Q_OBJECT
public:
    explicit AdoptRequestService(INetworkClient& networkClient, QObject* parent = nullptr);

    void createAdoptRequest(qint64 animalId);
    void getAdoptRequests(qint64 orgId, int page = 1);
    void acceptAdoptRequest(qint64 id);
    void denyAdoptRequest(qint64 id);

signals:
    void createAdoptRequestSuccess(const models::AdoptRequestDTO& result);
    void getAdoptRequestsSuccess(const models::AdoptRequestListDTO& result);
    void acceptAdoptRequestSuccess();
    void denyAdoptRequestSuccess();

    void createAdoptRequestFailed(QSharedPointer<services::BaseError> error);
    void getAdoptRequestsFailed(QSharedPointer<services::BaseError> error);
    void acceptAdoptRequestFailed(QSharedPointer<services::BaseError> error);
    void denyAdoptRequestFailed(QSharedPointer<services::BaseError> error);

private:
    void handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError);
    void handleSuccess(
        QNetworkReply& reply,
        std::function<void(const QJsonObject&)> onSuccess,
        std::function<void(QSharedPointer<BaseError>)> onError
    );

    INetworkClient& m_networkClient;
};

}  // namespace pawspective::services
