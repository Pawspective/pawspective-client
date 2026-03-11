#pragma once

#include <QJsonObject>
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

}  // namespace pawspective::models