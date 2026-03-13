#pragma once

#include <QJsonArray>
#include <QList>
#include <QObject>

#include "models/city_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

class CityService : public QObject {
    Q_OBJECT
public:
    explicit CityService(INetworkClient& networkClient, QObject* parent = nullptr);

    void getCities();

signals:
    void getCitiesSuccess(const QList<models::CityDTO>& cities);
    void getCitiesFailed(QSharedPointer<services::BaseError> error);

private:
    void handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError);
    void handleSuccess(
        QNetworkReply& reply,
        std::function<void(const QJsonArray&)> onSuccess,
        std::function<void(QSharedPointer<BaseError>)> onError
    );

    INetworkClient& m_networkClient;
};

}  // namespace pawspective::services
