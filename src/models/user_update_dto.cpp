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

UserUpdateDTO UserUpdateDTO::fromJson(const QJsonObject& json) {
    UserUpdateDTO dto;

    if (json.contains("email") && json["email"].isString() && !json["email"].isNull()) {
        dto.email = json["email"].toString();
    }
    if (json.contains("password") && json["password"].isString() && !json["password"].isNull()) {
        dto.password = json["password"].toString();
    }
    if (json.contains("first_name") && json["first_name"].isString() && !json["first_name"].isNull()) {
        dto.firstName = json["first_name"].toString();
    }
    if (json.contains("last_name") && json["last_name"].isString() && !json["last_name"].isNull()) {
        dto.lastName = json["last_name"].toString();
    }

    return dto;
}

}  // namespace pawspective::models