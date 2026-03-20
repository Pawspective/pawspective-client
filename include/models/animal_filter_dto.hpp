#pragma once

#include <QJsonObject>
#include <QVector>
#include <optional>

#include "animal_enums.hpp"

namespace pawspective::models {

struct AnimalFilterDTO {
    std::optional<QVector<int64_t>> breeds;
    std::optional<QVector<AnimalType>> animalTypes;
    std::optional<QVector<AnimalSize>> sizes;
    std::optional<QVector<AnimalGender>> genders;
    std::optional<QVector<CareLevel>> careLevels;
    std::optional<QVector<AnimalColor>> colors;
    std::optional<QVector<GoodWith>> goodWiths;
    std::optional<int> ageLte;
    std::optional<int> ageGte;

    QJsonObject toJson() const;
    static AnimalFilterDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
