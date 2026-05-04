#pragma once

#include <QJsonObject>
#include <QList>
#include <QMetaType>
#include <QString>
#include <optional>
#include "city_dto.hpp"

namespace pawspective::models {

struct OrganizationDTO {
    qint64 id = 0;
    QString name;
    // std::optional<QString> avatarUrl;
    std::optional<QString> description;
    CityDTO city;

    QJsonObject toJson() const;
    static OrganizationDTO fromJson(const QJsonObject& json);
};

struct OrganizationListDTO {
    QList<OrganizationDTO> items;
    int page{};
    int limit{};
    qint64 totalCount{};
    qint64 totalPages{};

    static OrganizationListDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models

Q_DECLARE_METATYPE(pawspective::models::OrganizationListDTO)