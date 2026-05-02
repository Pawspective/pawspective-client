
#include "../include/models/organization_dto.hpp"
#include <QString>
#include <optional>
#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject OrganizationDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["name"] = name;
    if (description.has_value()) {
        json["description"] = description.value();
    }
    json["city"] = city.toJson();

    return json;
}

OrganizationDTO OrganizationDTO::fromJson(const QJsonObject& json) {
    OrganizationDTO dto;

    if (!json.contains("id") || json["id"].isNull() || !json["id"].isDouble()) {
        throw std::invalid_argument("Invalid or missing id field");
    }
    dto.id = json["id"].toVariant().toLongLong();
    if (!json.contains("name") || json["name"].isNull() || !json["name"].isString()) {
        throw std::invalid_argument("Invalid or missing name field");
    }
    dto.name = json["name"].toString();
    if (json.contains("description") && json["description"].isString() && !json["description"].isNull()) {
        dto.description = json["description"].toString();
    }
    dto.city = CityDTO::fromJson(json["city"].toObject());

    return dto;
}

OrganizationListDTO OrganizationListDTO::fromJson(const QJsonObject& json) {
    OrganizationListDTO dto;
    dto.page = pawspective::utils::json::getRequiredInt32(json, "page");
    dto.limit = pawspective::utils::json::getRequiredInt32(json, "limit");
    dto.totalCount = pawspective::utils::json::getRequiredInt64(json, "total_count");
    dto.totalPages = pawspective::utils::json::getRequiredInt64(json, "total_pages");

    const QJsonArray items = json["items"].toArray();
    for (const auto& item : items) {
        dto.items.append(OrganizationDTO::fromJson(item.toObject()));
    }
    return dto;
}

}  // namespace pawspective::models