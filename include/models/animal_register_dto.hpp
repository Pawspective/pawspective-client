#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

#include "animal_enums.hpp"

namespace pawspective::models {

struct AnimalRegisterDTO {
    qint64 organizationId = 0;
    QString name;
    qint64 breedId = 0;
    AnimalSize size;
    AnimalGender gender;
    CareLevel careLevel;
    AnimalColor color;
    GoodWith goodWith;
    qint32 age = 0;
    std::optional<QString> description;
    AnimalStatus status;

    QJsonObject toJson() const;
    static AnimalRegisterDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
