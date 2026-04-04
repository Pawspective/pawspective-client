#include "../../include/viewmodels/create_animal_viewmodel.hpp"

namespace pawspective::viewmodels {

CreateAnimalViewModel::CreateAnimalViewModel(
    services::AnimalService& animalService,
    services::BreedService& breedService,
    QObject* parent
)
    : BaseViewModel(parent), m_animalService(animalService), m_breedService(breedService) {
    m_registerDto.status = models::AnimalStatus::Available;
    m_registerDto.age = 0;
    m_registerDto.breedId = 0;

    setupConnections();
}

void CreateAnimalViewModel::setupConnections() {
    connect(
        &m_animalService,
        &services::AnimalService::createAnimalSuccess,
        this,
        &CreateAnimalViewModel::onAnimalCreated
    );

    connect(&m_animalService, &services::AnimalService::createAnimalFailed, this, &CreateAnimalViewModel::onError);

    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFiltersSuccess,
        this,
        &CreateAnimalViewModel::onFiltersLoaded
    );

    connect(&m_animalService, &services::AnimalService::getAnimalFiltersFailed, this, &CreateAnimalViewModel::onError);

    connect(
        &m_breedService,
        &services::BreedService::getBreedsByTypeSuccess,
        this,
        &CreateAnimalViewModel::onBreedsLoaded
    );

    connect(
        &m_breedService,
        &services::BreedService::getBreedsByTypeFailed,
        this,
        &CreateAnimalViewModel::onBreedsLoadFailed
    );
}

void CreateAnimalViewModel::setAnimalType(const QString& value) {
    std::optional<models::AnimalType> newType;
    if (!value.isEmpty()) {
        newType = models::animalTypeFromApi(value.toLower());
    }

    if (m_animalType != newType) {
        m_animalType = newType;

        if (m_registerDto.breedId != 0) {
            m_registerDto.breedId = 0;
            emit breedIdChanged();
        }

        m_breedsList.clear();
        emit breedsChanged();

        if (newType.has_value()) {
            loadBreedsForType(newType.value());
        }

        emit animalTypeChanged();
        emit isBreedEnabledChanged();
    }
}

void CreateAnimalViewModel::loadBreedsForType(models::AnimalType type) {
    m_isLoadingBreeds = true;
    emit isBreedEnabledChanged();
    m_breedService.getBreedsByType(type);
}

void CreateAnimalViewModel::onBreedsLoaded(const QList<models::BreedDTO>& breeds) {
    updateBreedsList(breeds);
    m_isLoadingBreeds = false;
    emit isBreedEnabledChanged();
    setIsBusy(false);
}

void CreateAnimalViewModel::onBreedsLoadFailed(QSharedPointer<services::BaseError> error) {
    m_isLoadingBreeds = false;
    emit isBreedEnabledChanged();
    emitError(NetworkError, error->getMessage());
    setIsBusy(false);
}

void CreateAnimalViewModel::updateBreedsList(const QList<models::BreedDTO>& breeds) {
    QVariantList list;

    QVariantMap emptyEntry;
    emptyEntry["text"] = "";
    emptyEntry["value"] = "";
    list.append(emptyEntry);

    for (const auto& breed : breeds) {
        QVariantMap entry;
        entry["text"] = breed.name;
        entry["value"] = QString::number(breed.id);
        list.append(entry);
    }
    updateProperty(m_breedsList, list, [this] { emit breedsChanged(); });
}

void CreateAnimalViewModel::onFiltersLoaded(const models::AnimalFilterDTO& filters) {
    m_filterDto = filters;

    emit animalTypesChanged();
    emit sizesChanged();
    emit gendersChanged();
    emit careLevelsChanged();
    emit colorsChanged();
    emit goodWithsChanged();

    setIsBusy(false);
}

bool CreateAnimalViewModel::validateRequiredFields() {
    if (!m_animalType.has_value()) {
        emitError(ValidationError, "Please select an animal type.");
        return false;
    }

    if (m_registerDto.breedId <= 0 && m_animalType.value() != models::AnimalType::Other) {
        emitError(ValidationError, "Please select a breed.");
        return false;
    }

    if (!m_size.has_value()) {
        emitError(ValidationError, "Please select a size.");
        return false;
    }

    if (!m_gender.has_value()) {
        emitError(ValidationError, "Please select a gender.");
        return false;
    }

    if (!m_careLevel.has_value()) {
        emitError(ValidationError, "Please select a care level.");
        return false;
    }

    if (!m_color.has_value()) {
        emitError(ValidationError, "Please select a color.");
        return false;
    }

    if (!m_goodWith.has_value()) {
        emitError(ValidationError, "Please select good with option.");
        return false;
    }

    return true;
}

void CreateAnimalViewModel::fillRegisterDto() {
    if (m_size.has_value()) {
        m_registerDto.size = m_size.value();
    }
    if (m_gender.has_value()) {
        m_registerDto.gender = m_gender.value();
    }
    if (m_careLevel.has_value()) {
        m_registerDto.careLevel = m_careLevel.value();
    }
    if (m_color.has_value()) {
        m_registerDto.color = m_color.value();
    }
    if (m_goodWith.has_value()) {
        m_registerDto.goodWith = m_goodWith.value();
    }
}

void CreateAnimalViewModel::onAnimalCreated() {
    setIsBusy(false);
    emit creationFinished(true);
}

void CreateAnimalViewModel::onError(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);

    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        emitError(
            ValidationError,
            validationError->getErrors().empty()
                ? validationError->getMessage()
                : QString::fromStdString(validationError->getErrors()[0].errorMessage)
        );
    } else {
        emitError(NetworkError, error->getMessage());
    }

    emit creationFinished(false);
}

void CreateAnimalViewModel::createAnimal() {
    if (!validateRequiredFields()) {
        emit creationFinished(false);
        return;
    }

    fillRegisterDto();
    setIsBusy(true);
    m_animalService.createAnimal(m_registerDto);
}

void CreateAnimalViewModel::loadFilters() {
    setIsBusy(true);
    m_animalService.getAnimalFilters();
}

void CreateAnimalViewModel::initialize() { loadFilters(); }

}  // namespace pawspective::viewmodels