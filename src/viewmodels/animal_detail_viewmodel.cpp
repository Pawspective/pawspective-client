#include "viewmodels/animal_detail_viewmodel.hpp"

#include "models/animal_enums.hpp"

namespace pawspective::viewmodels {

AnimalDetailViewModel::AnimalDetailViewModel(QObject* parent) : QObject(parent) {}

AnimalDetailViewModel::AnimalDetailViewModel(const models::AnimalDTO& dto, QObject* parent) : QObject(parent) {
    setFromDTO(dto);
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
}

}  // namespace pawspective::viewmodels
