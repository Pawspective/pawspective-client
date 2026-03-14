#include "../include/models/city_dto.hpp"
#include <QString>

namespace pawspective::models {

QJsonObject CityDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["name"] = name;

    return json;
}

CityDTO CityDTO::fromJson(const QJsonObject& json) {
    CityDTO dto;

    if (!json.contains("id") || json["id"].isNull() || !json["id"].isDouble()) {
        throw std::invalid_argument("Invalid or missing id field");
    }
    dto.id = json["id"].toVariant().toLongLong();
    if (!json.contains("name") || json["name"].isNull() || !json["name"].isString()) {
        throw std::invalid_argument("Invalid or missing name field");
    }
    dto.name = json["name"].toString();

    return dto;
}

}  // namespace pawspective::models