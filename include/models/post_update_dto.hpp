#pragma once

#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <optional>

namespace pawspective::models {

struct PostUpdateDTO {
    std::optional<QString> text;
    std::optional<QStringList> photos;

    QJsonObject toJson() const;
    static PostUpdateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
