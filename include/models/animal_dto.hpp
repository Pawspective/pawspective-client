#pragma once

#include <QJsonObject>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QStringList>
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
    QStringList photos;
    AnimalStatus status;
    bool canBeAdopted = false;

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

Q_DECLARE_METATYPE(pawspective::models::AnimalListDTO)
