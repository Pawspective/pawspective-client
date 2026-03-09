#include "../include/models/organization_update_dto.hpp"

namespace pawspective::models {

QJsonObject OrganizationUpdateDTO::toJson() const {
    QJsonObject json;

    if (name.has_value()) {
        json["name"] = name.value();
    }
    if (description.has_value()) {
        json["description"] = description.value();
    }
    if (cityId.has_value()) {
        json["city"] = cityId.value();
    }
    return json;
}

static std::optional<QString> readOptionalField(const QJsonObject& json, const QString& key) {
    if (json.contains(key) && json[key].isString() && !json[key].isNull()) {
        return json[key].toString();
    }
    return std::nullopt;
}

OrganizationUpdateDTO OrganizationUpdateDTO::fromJson(const QJsonObject& json) {
    OrganizationUpdateDTO dto;

    dto.name = readOptionalField(json, "name");
    dto.description = readOptionalField(json, "description");
    if (json.contains("city_id") && !json["city_id"].isNull()) {
        if (!json["city_id"].isDouble()) {
            throw std::invalid_argument("city_id must be a number");
        }
        dto.cityId = json["city_id"].toVariant().toLongLong();
    }
    return dto;
}

}  // namespace pawspective::models