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

    QString gender = models::toApiString(dto.gender);
    if (m_gender != gender) {
        m_gender = gender;
        emit genderChanged();
    }

    QString size = models::toApiString(dto.size);
    if (m_size != size) {
        m_size = size;
        emit sizeChanged();
    }

    QString careLevel = models::toApiString(dto.careLevel);
    if (m_careLevel != careLevel) {
        m_careLevel = careLevel;
        emit careLevelChanged();
    }

    QString color = models::toApiString(dto.color);
    if (m_color != color) {
        m_color = color;
        emit colorChanged();
    }

    QString goodWith = models::toApiString(dto.goodWith);
    if (m_goodWith != goodWith) {
        m_goodWith = goodWith;
        emit goodWithChanged();
    }

    QString status = models::toApiString(dto.status);
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }

    QString desc = dto.description.value_or(QString{});
    if (m_description != desc) {
        m_description = desc;
        emit descriptionChanged();
    }
}

}  // namespace pawspective::viewmodels
