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

QString UserDTO::readString(const QJsonObject& json, const QString& key) {
    if (!json.contains(key) || json[key].isNull() || !json[key].isString()) {
        throw std::invalid_argument("Invalid or missing " + key.toStdString() + " field");
    }
    return json[key].toString();
}

UserDTO UserDTO::fromJson(const QJsonObject& json) {
    UserDTO dto;

    if (!json.contains("id") || json["id"].isNull() || !json["id"].isDouble()) {
        throw std::invalid_argument("Invalid or missing id field");
    }
    dto.id = json["id"].toVariant().toLongLong();
    dto.email = readString(json, "email");
    dto.firstName = readString(json, "first_name");
    dto.lastName = readString(json, "last_name");
    if (json.contains("organization_id") && !json["organization_id"].isNull()) {
        if (!json["organization_id"].isDouble()) {
            throw std::invalid_argument("organization_id must be a number");
        }
        dto.organizationId = json["organization_id"].toVariant().toLongLong();
    }

    return dto;
}

}  // namespace pawspective::models