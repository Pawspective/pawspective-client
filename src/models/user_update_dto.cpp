#include "../include/models/user_update_dto.hpp"

namespace pawspective::models {

QJsonObject UserUpdateDTO::toJson() const {
    QJsonObject json;

    if (email.has_value()) {
        json["email"] = email.value();
    }
    if (password.has_value()) {
        json["password"] = password.value();
    }
    if (firstName.has_value()) {
        json["first_name"] = firstName.value();
    }
    if (lastName.has_value()) {
        json["last_name"] = lastName.value();
    }
    return json;
}

static std::optional<QString> readOptionalField(const QJsonObject& json, const QString& key) {
    if (json.contains(key) && json[key].isString() && !json[key].isNull()) {
        return json[key].toString();
    }
    return std::nullopt;
}

UserUpdateDTO UserUpdateDTO::fromJson(const QJsonObject& json) {
    UserUpdateDTO dto;

    dto.email = readOptionalField(json, "email");
    dto.password = readOptionalField(json, "password");
    dto.firstName = readOptionalField(json, "first_name");
    dto.lastName = readOptionalField(json, "last_name");

    return dto;
}

}  // namespace pawspective::models