#pragma once

#include <QJsonObject>
#include <QList>
#include <QMetaType>
#include <QString>

#include "animal_dto.hpp"

namespace pawspective::models {

struct AdoptRequestDTO {
    qint64 id{};
    QString email;
    AnimalDTO animal;

    QJsonObject toJson() const;
    static AdoptRequestDTO fromJson(const QJsonObject& json);
};

struct AdoptRequestListDTO {
    QList<AdoptRequestDTO> items;
    int page{};
    int limit{};
    qint64 totalCount{};
    qint64 totalPages{};

    static AdoptRequestListDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models

Q_DECLARE_METATYPE(pawspective::models::AdoptRequestDTO)
Q_DECLARE_METATYPE(pawspective::models::AdoptRequestListDTO)
