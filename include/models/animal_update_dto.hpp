#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

#include "animal_enums.hpp"

namespace pawspective::models {

struct AnimalUpdateDTO {
    std::optional<QString> name;
    std::optional<qint64> breedId;
    std::optional<AnimalSize> size;
    std::optional<AnimalGender> gender;
    std::optional<CareLevel> careLevel;
    std::optional<AnimalColor> color;
    std::optional<GoodWith> goodWith;
    std::optional<qint32> age;
    std::optional<QString> description;
    std::optional<AnimalStatus> status;

    QJsonObject toJson() const;
    static AnimalUpdateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
