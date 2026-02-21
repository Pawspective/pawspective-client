#include "../include/models/user_dto.hpp"
#include <QString>
#include <optional>

namespace pawspective::models {

QJsonObject UserDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["email"] = email;
    json["first_name"] = firstName;
    json["last_name"] = lastName;
    if (organizationId.has_value()) {
        json["organization_id"] = organizationId.value();
    }

    return json;
}

UserDTO UserDTO::fromJson(const QJsonObject& json) {
    UserDTO dto;

    if (!json.contains("id") || json["id"].isNull() || !json["id"].isDouble()) {
        throw std::invalid_argument("Invalid or missing id field");
    }
    dto.id = json["id"].toVariant().toLongLong();
    if (!json.contains("email") || json["email"].isNull() || !json["email"].isString()) {
        throw std::invalid_argument("Invalid or missing email field");
    }
    dto.email = json["email"].toString();
    if (!json.contains("first_name") || json["first_name"].isNull() || !json["first_name"].isString()) {
        throw std::invalid_argument("Invalid or missing first_name field");
    }
    dto.firstName = json["first_name"].toString();
    if (!json.contains("last_name") || json["last_name"].isNull() || !json["last_name"].isString()) {
        throw std::invalid_argument("Invalid or missing last_name field");
    }
    dto.lastName = json["last_name"].toString();
    if (json.contains("organization_id") && !json["organization_id"].isNull()) {
        if (!json["organization_id"].isDouble()) {
            throw std::invalid_argument("organization_id must be a number");
        }
        dto.organizationId = json["organization_id"].toVariant().toLongLong();
    }

    return dto;
}

}  // namespace pawspective::models