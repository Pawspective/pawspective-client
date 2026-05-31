#include "viewmodels/animal_detail_viewmodel.hpp"

#include "models/animal_enums.hpp"
#include "services/errors.hpp"

namespace pawspective::viewmodels {

AnimalDetailViewModel::AnimalDetailViewModel(
    services::AnimalService& animalService,
    services::OrganizationService& organizationService,
    services::AdoptRequestService& adoptRequestService,
    QObject* parent
)
    : BaseViewModel(parent),
      m_animalService(animalService),
      m_organizationService(organizationService),
      m_adoptRequestService(adoptRequestService) {
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalSuccess,
        this,
        [this](const models::AnimalDTO& animal) {
            if (animal.id != m_currentAnimalId) {
                return;
            }
            setFromDTO(animal);
            if (m_organizationId > 0) {
                m_organizationService.getOrganization(m_organizationId);
            } else {
                setIsBusy(false);
            }
        }
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            setIsBusy(false);
            if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
                emitError(ValidationError, formatValidationError(validationError));
            } else {
                emitError(NetworkError, error->getMessage());
            }
        }
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::getOrganizationSuccess,
        this,
        [this](const models::OrganizationDTO& org) {
            setIsBusy(false);
            setFromOrgDTO(org);
        }
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::getOrganizationFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            setIsBusy(false);
            emitError(NetworkError, error->getMessage());
        }
    );
    connect(
        &m_animalService,
        &services::AnimalService::deleteAnimalSuccess,
        this,
        &AnimalDetailViewModel::handleDeleteSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::deleteAnimalFailed,
        this,
        &AnimalDetailViewModel::handleDeleteFailed
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::createAdoptRequestSuccess,
        this,
        [this](const models::AdoptRequestDTO&) {
            if (m_canBeAdopted) {
                m_canBeAdopted = false;
                emit canBeAdoptedChanged();
            }
            emit adoptSuccess();
        }
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::createAdoptRequestFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            QString message = error ? error->getMessage() : "Failed to send adoption request";
            emit adoptFailed(message);
            emitError(ErrorType::NetworkError, message);
        }
    );
}

void AnimalDetailViewModel::loadAnimal(qint64 id) {
    m_currentAnimalId = id;
    setIsBusy(true);
    m_animalService.getAnimal(id);
}

void AnimalDetailViewModel::setFromDTO(const models::AnimalDTO& dto) {
    if (m_name != dto.name) {
        m_name = dto.name;
        emit nameChanged();
    }

    QString type = models::toApiString(dto.breed.animalType);
    if (m_animalType != type) {
        m_animalType = type;
        emit animalTypeChanged();
    }

    if (m_breedName != dto.breed.name) {
        m_breedName = dto.breed.name;
        emit breedNameChanged();
    }

    if (m_age != dto.age) {
        m_age = dto.age;
        emit ageChanged();
    }

    QString newGender = models::toApiString(dto.gender);
    if (m_gender != newGender) {
        m_gender = newGender;
        emit genderChanged();
    }

    QString newSize = models::toApiString(dto.size);
    if (m_size != newSize) {
        m_size = newSize;
        emit sizeChanged();
    }

    QString newCareLevel = models::toApiString(dto.careLevel);
    if (m_careLevel != newCareLevel) {
        m_careLevel = newCareLevel;
        emit careLevelChanged();
    }

    QString newColor = models::toApiString(dto.color);
    if (m_color != newColor) {
        m_color = newColor;
        emit colorChanged();
    }

    QString newGoodWith = models::toApiString(dto.goodWith);
    if (m_goodWith != newGoodWith) {
        m_goodWith = newGoodWith;
        emit goodWithChanged();
    }

    QString newStatus = models::toApiString(dto.status);
    if (m_status != newStatus) {
        m_status = newStatus;
        emit statusChanged();
    }

    QString desc = dto.description.value_or(QString{});
    if (m_description != desc) {
        m_description = desc;
        emit descriptionChanged();
    }

    if (m_canBeAdopted != dto.canBeAdopted) {
        m_canBeAdopted = dto.canBeAdopted;
        emit canBeAdoptedChanged();
    }

    if (m_organizationId != dto.organizationId) {
        m_organizationId = dto.organizationId;
        emit organizationIdChanged();

        m_organizationName.clear();
        emit organizationNameChanged();
        m_organizationCity.clear();
        emit organizationCityChanged();
        m_organizationDescription.clear();
        emit organizationDescriptionChanged();
    }
}

void AnimalDetailViewModel::setFromOrgDTO(const models::OrganizationDTO& dto) {
    if (m_organizationName != dto.name) {
        m_organizationName = dto.name;
        emit organizationNameChanged();
    }

    QString city = dto.city.name;
    if (m_organizationCity != city) {
        m_organizationCity = city;
        emit organizationCityChanged();
    }

    QString desc = dto.description.value_or(QString{});
    if (m_organizationDescription != desc) {
        m_organizationDescription = desc;
        emit organizationDescriptionChanged();
    }
}

void AnimalDetailViewModel::deleteAnimal() {
    if (m_currentAnimalId == 0) {
        emit deleteFailed("No animal selected");
        return;
    }
    m_animalService.deleteAnimal(m_currentAnimalId);
}

void AnimalDetailViewModel::adoptAnimal() {
    if (m_currentAnimalId == 0) {
        emit adoptFailed("No animal selected");
        return;
    }
    m_adoptRequestService.createAdoptRequest(m_currentAnimalId);
}

void AnimalDetailViewModel::handleDeleteSuccess() { emit deleteSuccess(); }

void AnimalDetailViewModel::handleDeleteFailed(QSharedPointer<services::BaseError> error) {
    QString message = error ? error->getMessage() : "Failed to delete animal";
    emit deleteFailed(message);
    emitError(ErrorType::NetworkError, message);
}

}  // namespace pawspective::viewmodels
