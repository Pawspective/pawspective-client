#include "viewmodels/animal_card_viewmodel.hpp"

#include "models/animal_enums.hpp"

namespace pawspective::viewmodels {

AnimalCardViewModel::AnimalCardViewModel(QObject* parent) : QObject(parent) {}

AnimalCardViewModel::AnimalCardViewModel(const models::AnimalDTO& dto, QObject* parent) : QObject(parent) {
    setFromDTO(dto);
}

void AnimalCardViewModel::setFromDTO(const models::AnimalDTO& dto) {
    if (m_name != dto.name) {
        m_name = dto.name;
        emit nameChanged();
    }

    if (m_age != dto.age) {
        m_age = dto.age;
        emit ageChanged();
    }

    QString type = models::toApiString(dto.breed.animalType);
    if (m_animalType != type) {
        m_animalType = type;
        emit animalTypeChanged();
    }

    QString desc = dto.description.value_or(QString{});
    if (m_description != desc) {
        m_description = desc;
        emit descriptionChanged();
    }
}

}  // namespace pawspective::viewmodels
