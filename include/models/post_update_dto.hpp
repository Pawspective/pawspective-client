#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct PostUpdateDTO {
    std::optional<QString> text;

    QJsonObject toJson() const;
    static PostUpdateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
