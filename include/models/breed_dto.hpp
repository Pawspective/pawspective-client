#pragma once

#include <QJsonObject>
#include <QString>
#include "animal_enums.hpp"

namespace pawspective::models {

struct BreedDTO {
    qint64 id = 0;
    AnimalType animalType = AnimalType::Other;
    QString name;

    QJsonObject toJson() const;
    static BreedDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
