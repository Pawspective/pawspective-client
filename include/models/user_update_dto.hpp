#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct UserUpdateDTO {
    std::optional<QString> email;
    std::optional<QString> password;
    std::optional<QString> firstName;
    std::optional<QString> lastName;

    QJsonObject toJson() const;
    static UserUpdateDTO fromJson(const QJsonObject& json);

private:
    std::optional<QString> readStringField(const QJsonObject& json, const QString& key) const;
};

}  // namespace pawspective::models