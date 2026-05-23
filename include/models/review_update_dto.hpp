#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct ReviewUpdateDTO {
    std::optional<QString> text;

    QJsonObject toJson() const;
    static ReviewUpdateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
