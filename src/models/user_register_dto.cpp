#include "../include/models/user_register_dto.hpp"
#include <QString>

namespace pawspective::models {

QJsonObject UserRegisterDTO::toJson() const {
    QJsonObject json;

    json["email"] = email;
    json["first_name"] = firstName;
    json["last_name"] = lastName;
    json["password"] = password;

    return json;
}

UserRegisterDTO UserRegisterDTO::fromJson(const QJsonObject& json) {
    UserRegisterDTO dto;

    if (!json.contains("email") || !json["email"].isString()) {
        throw std::invalid_argument("Invalid or missing email field");
    }
    dto.email = json["email"].toString();
    if (!json.contains("first_name") || !json["first_name"].isString()) {
        throw std::invalid_argument("Invalid or missing first_name field");
    }
    dto.firstName = json["first_name"].toString();
    if (!json.contains("last_name") || !json["last_name"].isString()) {
        throw std::invalid_argument("Invalid or missing last_name field");
    }
    dto.lastName = json["last_name"].toString();
    if (!json.contains("password") || !json["password"].isString()) {
        throw std::invalid_argument("Invalid or missing password field");
    }
    dto.password = json["password"].toString();

    return dto;
}

}  // namespace pawspective::models