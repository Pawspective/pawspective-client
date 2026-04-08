#include "../../include/viewmodels/update_animal_viewmodel.hpp"

namespace pawspective::viewmodels {

UpdateAnimalViewModel::UpdateAnimalViewModel(
    services::AnimalService& animalService,
    services::BreedService& breedService,
    QObject* parent
)
    : BaseViewModel(parent), m_animalService(animalService), m_breedService(breedService) {
    setupConnections();
}

void UpdateAnimalViewModel::setupConnections() {
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalSuccess,
        this,
        &UpdateAnimalViewModel::handleGetSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFailed,
        this,
        &UpdateAnimalViewModel::handleGetFailed
    );
    connect(
        &m_animalService,
        &services::AnimalService::updateAnimalSuccess,
        this,
        &UpdateAnimalViewModel::handleUpdateSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::updateAnimalFailed,
        this,
        &UpdateAnimalViewModel::handleUpdateFailed
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFiltersSuccess,
        this,
        &UpdateAnimalViewModel::handleFiltersLoaded
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFiltersFailed,
        this,
        &UpdateAnimalViewModel::handleFiltersFailed
    );
    connect(
        &m_breedService,
        &services::BreedService::getBreedsByTypeSuccess,
        this,
        &UpdateAnimalViewModel::handleBreedsLoaded
    );
    connect(
        &m_breedService,
        &services::BreedService::getBreedsByTypeFailed,
        this,
        &UpdateAnimalViewModel::handleBreedsLoadFailed
    );
}

void UpdateAnimalViewModel::initialize() {
    if (isBusy()) {
        return;
    }
    setIsBusy(true);
    loadFilters();
    if (m_animalId > 0) {
        m_animalService.getAnimal(m_animalId);
    } else {
        setIsBusy(false);
        emit loadFailed("Invalid animal ID");
    }
}

void UpdateAnimalViewModel::cleanup() {
    setIsBusy(false);
    discardChanges();
    m_breedsList.clear();
    m_isLoadingBreeds = false;
    m_animalId = 0;
    m_currentAnimalType = std::nullopt;
    m_originalData = models::AnimalDTO{};
    m_filterDto = models::AnimalFilterDTO{};
}

QString UpdateAnimalViewModel::animalType() const {
    if (m_currentAnimalType.has_value()) {
        return models::toApiString(m_currentAnimalType.value());
    }
    if (m_originalData.breed.id > 0) {
        return models::toApiString(m_originalData.breed.animalType);
    }
    return "";
}

qint64 UpdateAnimalViewModel::breedId() const {
    return m_changes.breedId.value_or(m_originalData.breed.id);
}

QString UpdateAnimalViewModel::size() const {
    if (m_changes.size.has_value()) {
        return models::toApiString(m_changes.size.value());
    }
    return models::toApiString(m_originalData.size);
}

QString UpdateAnimalViewModel::gender() const {
    if (m_changes.gender.has_value()) {
        return models::toApiString(m_changes.gender.value());
    }
    return models::toApiString(m_originalData.gender);
}

int UpdateAnimalViewModel::age() const {
    return m_changes.age.value_or(m_originalData.age);
}

QString UpdateAnimalViewModel::careLevel() const {
    if (m_changes.careLevel.has_value()) {
        return models::toApiString(m_changes.careLevel.value());
    }
    return models::toApiString(m_originalData.careLevel);
}

QString UpdateAnimalViewModel::color() const {
    if (m_changes.color.has_value()) {
        return models::toApiString(m_changes.color.value());
    }
    return models::toApiString(m_originalData.color);
}

QString UpdateAnimalViewModel::goodWith() const {
    if (m_changes.goodWith.has_value()) {
        return models::toApiString(m_changes.goodWith.value());
    }
    return models::toApiString(m_originalData.goodWith);
}

QString UpdateAnimalViewModel::status() const {
    if (m_changes.status.has_value()) {
        return models::toApiString(m_changes.status.value());
    }
    return models::toApiString(m_originalData.status);
}

bool UpdateAnimalViewModel::isBreedEnabled() const {
    auto type = m_currentAnimalType.has_value() 
        ? m_currentAnimalType.value() 
        : m_originalData.breed.animalType;
    return !m_isLoadingBreeds;
}

QVariantList UpdateAnimalViewModel::statuses() const {
    QVariantList list;
    
    QVariantMap available;
    available["text"] = "available";
    available["value"] = "available";
    list.append(available);
    
    QVariantMap adopted;
    adopted["text"] = "adopted";
    adopted["value"] = "adopted";
    list.append(adopted);
    
    QVariantMap unavailable;
    unavailable["text"] = "unavailable";
    unavailable["value"] = "unavailable";
    list.append(unavailable);
    
    return list;
}

void UpdateAnimalViewModel::setName(const QString& value) {
    if (name() != value) {
        auto original = m_originalData.name;
        m_changes.name = (value == original) ? std::nullopt : std::make_optional(value);
        emit nameChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setDescription(const QString& value) {
    if (description() != value) {
        QString original = m_originalData.description.value_or("");
        m_changes.description = (value == original) ? std::nullopt : std::make_optional(value);
        emit descriptionChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setAnimalType(const QString& value) {
    if (value.isEmpty()) {
        return;
    }
    
    auto newType = models::animalTypeFromApi(value.toLower());
    auto currentType = m_currentAnimalType.has_value() 
        ? m_currentAnimalType.value() 
        : m_originalData.breed.animalType;
    
    if (currentType != newType) {
        m_currentAnimalType = newType;
        
        if (breedId() != 0) {
            m_changes.breedId = std::nullopt;
            emit breedIdChanged();
        }
        m_breedsList.clear();
        emit breedsChanged();

        loadBreedsForType(newType);
        
        emit animalTypeChanged();
        emit isBreedEnabledChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setBreedId(qint64 value) {
    if (breedId() != value) {
        m_changes.breedId = (value == m_originalData.breed.id) ? std::nullopt : std::make_optional(value);
        emit breedIdChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setSize(const QString& value) {
    std::optional<models::AnimalSize> newSize;
    if (!value.isEmpty()) {
        newSize = models::animalSizeFromApi(value.toLower());
    }

    auto currentSize = m_changes.size.has_value()
        ? m_changes.size.value()
        : m_originalData.size;

    if (currentSize != newSize) {
        m_changes.size = newSize;
        emit sizeChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setGender(const QString& value) {
    std::optional<models::AnimalGender> newGender;
    if (!value.isEmpty()) {
        newGender = models::animalGenderFromApi(value.toLower());
    }

    auto currentGender = m_changes.gender.has_value()
        ? m_changes.gender.value()
        : m_originalData.gender;

    if (currentGender != newGender) {
        m_changes.gender = newGender;
        emit genderChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setAge(int value) {
    if (age() != value) {
        m_changes.age = (value == m_originalData.age) ? std::nullopt : std::make_optional(value);
        emit ageChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setCareLevel(const QString& value) {
    std::optional<models::CareLevel> newCareLevel;
    if (!value.isEmpty()) {
        newCareLevel = models::careLevelFromApi(value.toLower());
    }

    auto currentCareLevel = m_changes.careLevel.has_value()
        ? m_changes.careLevel.value()
        : m_originalData.careLevel;

    if (currentCareLevel != newCareLevel) {
        m_changes.careLevel = newCareLevel;
        emit careLevelChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setColor(const QString& value) {
    std::optional<models::AnimalColor> newColor;
    if (!value.isEmpty()) {
        newColor = models::animalColorFromApi(value.toLower());
    }

    auto currentColor = m_changes.color.has_value()
        ? m_changes.color.value()
        : m_originalData.color;

    if (currentColor != newColor) {
        m_changes.color = newColor;
        emit colorChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setGoodWith(const QString& value) {
    std::optional<models::GoodWith> newGoodWith;
    if (!value.isEmpty()) {
        newGoodWith = models::goodWithFromApi(value.toLower());
    }

    auto currentGoodWith = m_changes.goodWith.has_value()
        ? m_changes.goodWith.value()
        : m_originalData.goodWith;

    if (currentGoodWith != newGoodWith) {
        m_changes.goodWith = newGoodWith;
        emit goodWithChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::setStatus(const QString& value) {
    if (value.isEmpty()) {
        return;
    }
    
    auto newStatus = models::animalStatusFromApi(value.toLower());
    auto currentStatus = m_changes.status.has_value()
        ? m_changes.status.value()
        : m_originalData.status;

    if (currentStatus != newStatus) {
        m_changes.status = (newStatus == m_originalData.status) 
            ? std::nullopt 
            : std::make_optional(newStatus);
        emit statusChanged();
        updateDirtyStatus();
    }
}

void UpdateAnimalViewModel::loadFilters() {
    m_animalService.getAnimalFilters();
}

void UpdateAnimalViewModel::loadBreedsForType(models::AnimalType type) {
    m_isLoadingBreeds = true;
    emit isBreedEnabledChanged();
    m_breedService.getBreedsByType(type);
}

void UpdateAnimalViewModel::updateBreedsList(const QList<models::BreedDTO>& breeds) {
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

bool UpdateAnimalViewModel::validateRequiredFields() {
    if (breedId() <= 0) {
        emitError(ValidationError, "Please select a breed.");
        return false;
    }
    return true;
}

void UpdateAnimalViewModel::handleGetSuccess(const models::AnimalDTO& animal) {
    m_originalData = animal;

    if (animal.breed.id > 0) {
        m_currentAnimalType = animal.breed.animalType;
        loadBreedsForType(m_currentAnimalType.value());
    } else {
        m_currentAnimalType = models::AnimalType::Other;
        loadBreedsForType(models::AnimalType::Other);
    }
    
    discardChanges();
    setIsBusy(false);
    emit loadCompleted();
}

void UpdateAnimalViewModel::handleGetFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    QString msg = error ? error->getMessage() : "Failed to load animal data";
    emit loadFailed(msg);
    emitError(ErrorType::NetworkError, msg);
}

void UpdateAnimalViewModel::handleUpdateSuccess(const models::AnimalDTO& animal) {
    setIsBusy(false);
    m_originalData = animal;
    m_changes = models::AnimalUpdateDTO();
    setDirty(false);
    notifyAllChanged();
    emit saveCompleted();
}

void UpdateAnimalViewModel::handleUpdateFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (!error) {
        emitError(ErrorType::UnknownError, "An unexpected error occurred.");
        emit saveFailed("Unknown error");
        return;
    }

    QString message;
    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        message = validationError->getErrors().empty()
            ? "Please check the entered data."
            : QString::fromStdString(validationError->getErrors()[0].errorMessage);
        emitError(ErrorType::ValidationError, message);
    } else {
        message = error->getMessage();
        emitError(ErrorType::NetworkError, message);
    }

    emit saveFailed(message);
}

void UpdateAnimalViewModel::handleFiltersLoaded(const models::AnimalFilterDTO& filters) {
    m_filterDto = filters;
    emit animalTypesChanged();
    emit sizesChanged();
    emit gendersChanged();
    emit careLevelsChanged();
    emit colorsChanged();
    emit goodWithsChanged();
    emit statusesChanged();
}

void UpdateAnimalViewModel::handleFiltersFailed(QSharedPointer<services::BaseError> error) {
    QString msg = error ? error->getMessage() : "Failed to load filter options";
    emitError(ErrorType::NetworkError, msg);
}

void UpdateAnimalViewModel::handleBreedsLoaded(const QList<models::BreedDTO>& breeds) {
    updateBreedsList(breeds);
    m_isLoadingBreeds = false;
    emit isBreedEnabledChanged();
}

void UpdateAnimalViewModel::handleBreedsLoadFailed(QSharedPointer<services::BaseError> error) {
    m_isLoadingBreeds = false;
    emit isBreedEnabledChanged();
    emitError(NetworkError, error->getMessage());
}

void UpdateAnimalViewModel::saveChanges() {
    if (!m_isDirty || isBusy()) {
        return;
    }
    if (!validateRequiredFields()) {
        emit saveFailed("Please select a breed.");
        return;
    }
    
    setIsBusy(true);
    m_animalService.updateAnimal(m_animalId, m_changes);
}

void UpdateAnimalViewModel::discardChanges() {
    m_changes = models::AnimalUpdateDTO();
    m_currentAnimalType = m_originalData.breed.animalType;
    setDirty(false);
    notifyAllChanged();
}

void UpdateAnimalViewModel::updateDirtyStatus() {
    bool dirty = m_changes.name.has_value() ||
                 m_changes.description.has_value() ||
                 m_changes.breedId.has_value() ||
                 m_changes.size.has_value() ||
                 m_changes.gender.has_value() ||
                 m_changes.age.has_value() ||
                 m_changes.careLevel.has_value() ||
                 m_changes.color.has_value() ||
                 m_changes.goodWith.has_value() ||
                 m_changes.status.has_value();
    setDirty(dirty);
}

void UpdateAnimalViewModel::setDirty(bool dirty) {
    updateProperty(m_isDirty, dirty, [this] { emit dirtyChanged(); });
}

void UpdateAnimalViewModel::notifyAllChanged() {
    emit nameChanged();
    emit descriptionChanged();
    emit animalTypeChanged();
    emit breedIdChanged();
    emit sizeChanged();
    emit genderChanged();
    emit ageChanged();
    emit careLevelChanged();
    emit colorChanged();
    emit goodWithChanged();
    emit statusChanged();
    emit isBreedEnabledChanged();
}

}  // namespace pawspective::viewmodels