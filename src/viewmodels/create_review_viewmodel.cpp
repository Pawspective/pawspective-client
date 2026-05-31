#include "viewmodels/create_review_viewmodel.hpp"

#include <QVariantMap>

#include "models/animal_dto.hpp"
#include "services/animal_service.hpp"

namespace pawspective::viewmodels {

CreateReviewViewModel::CreateReviewViewModel(
    services::ReviewService& reviewService,
    services::AnimalService& animalService,
    QObject* parent
)
    : BaseViewModel(parent), m_reviewService(reviewService), m_animalService(animalService) {
    setupConnections();
}

void CreateReviewViewModel::setupConnections() {
    connect(
        &m_reviewService,
        &services::ReviewService::createReviewSuccess,
        this,
        &CreateReviewViewModel::onReviewCreated
    );

    connect(&m_reviewService, &services::ReviewService::createReviewFailed, this, &CreateReviewViewModel::onError);

    connect(
        &m_animalService,
        &services::AnimalService::getPendingReviewAnimalsSuccess,
        this,
        &CreateReviewViewModel::onAnimalsLoaded
    );
    connect(
        &m_animalService,
        &services::AnimalService::getPendingReviewAnimalsFailed,
        this,
        &CreateReviewViewModel::onAnimalsLoadFailed
    );
}

void CreateReviewViewModel::onAnimalsLoaded(const QList<models::AnimalDTO>& animals) {
    QVariantList availableAnimals;
    bool selectedAnimalAval = false;

    for (const auto& animal : animals) {
        QVariantMap item;
        item["id"] = animal.id;
        item["name"] = animal.name;
        availableAnimals.append(item);

        if (animal.id == m_createDto.animalId) {
            selectedAnimalAval = true;
        }
    }

    updateProperty(m_availableAnimals, availableAnimals, [this] { emit availableAnimalsChanged(); });

    if (!availableAnimals.isEmpty() && (!selectedAnimalAval || m_createDto.animalId <= 0)) {
        setAnimalId(availableAnimals.first().toMap().value("id").toLongLong());
    }

    setIsBusy(false);
}

void CreateReviewViewModel::onAnimalsLoadFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    onError(error);
}

void CreateReviewViewModel::onReviewCreated() {
    setIsBusy(false);
    emit creationFinished(true);
}

void CreateReviewViewModel::onError(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);

    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        emitError(ValidationError, formatValidationError(validationError));
    } else {
        emitError(NetworkError, error->getMessage());
    }

    emit creationFinished(false);
}

void CreateReviewViewModel::createReview() {
    setIsBusy(true);
    m_reviewService.createReview(m_createDto);
}

void CreateReviewViewModel::loadAvailableAnimals() {
    setIsBusy(true);
    m_animalService.getPendingReviewAnimals();
}

void CreateReviewViewModel::initialize() { loadAvailableAnimals(); }

void CreateReviewViewModel::cleanup() {
    m_createDto = models::ReviewCreateDTO{};
    m_availableAnimals.clear();
    setIsBusy(false);

    emit textChanged();
    emit animalIdChanged();
    emit availableAnimalsChanged();
}

}  // namespace pawspective::viewmodels