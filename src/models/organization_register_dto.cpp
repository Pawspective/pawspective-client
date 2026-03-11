#include "../include/models/organization_register_dto.hpp"
#include <QString>

namespace pawspective::models {

QJsonObject OrganizationRegisterDTO::toJson() const {
    QJsonObject json;

    json["name"] = name;
    if (description.has_value()) {
        json["description"] = description.value();
    }
    json["city_id"] = cityId;

    return json;
}

OrganizationRegisterDTO OrganizationRegisterDTO::fromJson(const QJsonObject& json) {
    OrganizationRegisterDTO dto;

    if (!json.contains("name") || !json["name"].isString()) {
        throw std::invalid_argument("Invalid or missing name field");
    }
    dto.name = json["name"].toString();
    if (json.contains("description") && json["description"].isString() && !json["description"].isNull()) {
        dto.description = json["description"].toString();
    }
    if (!json.contains("city_id") || json["city_id"].isNull() || !json["city_id"].isDouble()) {
        throw std::invalid_argument("Invalid or missing city_id field");
    }
    dto.cityId = json["city_id"].toVariant().toLongLong();

    return dto;
}

}  // namespace pawspective::models