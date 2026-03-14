#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct OrganizationUpdateDTO {
    std::optional<QString> name;
    // std::optional<QString> avatarUrl;
    std::optional<QString> description;
    std::optional<qint64> cityId = std::nullopt;

    QJsonObject toJson() const;
    static OrganizationUpdateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models