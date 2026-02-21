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

QString UserRegisterDTO::readString(const QJsonObject& json, const QString& key) {
    if (!json.contains(key) || !json[key].isString()) {
        throw std::invalid_argument("Invalid or missing " + key.toStdString() + " field");
    }
    return json[key].toString();
}

UserRegisterDTO UserRegisterDTO::fromJson(const QJsonObject& json) {
    UserRegisterDTO dto;

    dto.email = readString(json, "email");
    dto.firstName = readString(json, "first_name");
    dto.lastName = readString(json, "last_name");
    dto.password = readString(json, "password");

    return dto;
}

}  // namespace pawspective::models