#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include <optional>

#include "animal_enums.hpp"
#include "breed_dto.hpp"

namespace pawspective::models {

struct AnimalDTO {
    qint64 id = 0;
    qint64 organizationId = 0;
    QString name;
    BreedDTO breed;
    AnimalSize size;
    AnimalGender gender;
    CareLevel careLevel;
    AnimalColor color;
    GoodWith goodWith;
    qint32 age = 0;
    std::optional<QString> description;
    AnimalStatus status;

    QJsonObject toJson() const;
    static AnimalDTO fromJson(const QJsonObject& json);
};

struct AnimalListDTO {
    QList<AnimalDTO> items;
    int page{};
    int limit{};
    qint64 totalCount{};
    qint64 totalPages{};

    static AnimalListDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
