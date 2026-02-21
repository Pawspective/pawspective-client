#pragma once

#include <QJsonObject>
#include <QString>

namespace pawspective::models {

struct UserRegisterDTO {
    QString email;
    QString password;
    QString firstName;
    QString lastName;

    QJsonObject toJson() const;
    static UserRegisterDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models