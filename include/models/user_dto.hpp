#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct UserDTO {
    qint64 id = 0;
    QString email;
    QString firstName;
    QString lastName;
    std::optional<qint64> organizationId;

    QJsonObject toJson() const;
    static UserDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models