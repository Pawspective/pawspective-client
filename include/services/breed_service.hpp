#pragma once

#include <QJsonArray>
#include <QList>
#include <QObject>

#include "models/animal_enums.hpp"
#include "models/breed_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

class BreedService : public QObject {
    Q_OBJECT
public:
    explicit BreedService(INetworkClient& networkClient, QObject* parent = nullptr);

    void getBreedsByType(models::AnimalType type);

signals:
    void getBreedsByTypeSuccess(const QList<models::BreedDTO>& breeds);
    void getBreedsByTypeFailed(QSharedPointer<services::BaseError> error);

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
