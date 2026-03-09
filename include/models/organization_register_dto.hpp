#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct OrganizationRegisterDTO {
    QString name;
    //std::optional<QString> avatarUrl;
    std::optional<QString> description;
    qint64 cityId = 0;

    QJsonObject toJson() const;
    static OrganizationRegisterDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models