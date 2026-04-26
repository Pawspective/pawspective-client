#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>

#include "models/animal_dto.hpp"
#include "models/animal_filter_dto.hpp"
#include "models/animal_register_dto.hpp"
#include "models/animal_update_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

class AnimalService : public QObject {
    Q_OBJECT
public:
    explicit AnimalService(INetworkClient& networkClient, QObject* parent = nullptr);

    void getAnimals(const models::AnimalFilterDTO& filter);
    void getAnimal(qint64 id);
    void createAnimal(const models::AnimalRegisterDTO& dto);
    void updateAnimal(qint64 id, const models::AnimalUpdateDTO& dto);
    void getAnimalFilters();
    void getAnimalsByOrganization(qint64 organizationId);

signals:
    void getAnimalsSuccess(const models::AnimalListDTO& result);
    void getAnimalSuccess(const models::AnimalDTO& animal);
    void createAnimalSuccess(const models::AnimalDTO& animal);
    void updateAnimalSuccess(const models::AnimalDTO& animal);
    void getAnimalFiltersSuccess(const models::AnimalFilterDTO& filters);
    void getAnimalsByOrganizationSuccess(const QList<models::AnimalDTO>& animals);

    void getAnimalsFailed(QSharedPointer<services::BaseError> error);
    void getAnimalFailed(QSharedPointer<services::BaseError> error);
    void createAnimalFailed(QSharedPointer<services::BaseError> error);
    void updateAnimalFailed(QSharedPointer<services::BaseError> error);
    void getAnimalFiltersFailed(QSharedPointer<services::BaseError> error);
    void getAnimalsByOrganizationFailed(QSharedPointer<services::BaseError> error);

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
