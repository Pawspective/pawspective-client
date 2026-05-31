#pragma once

#include <QJsonObject>
#include <QString>

namespace pawspective::models {

struct ReviewCreateDTO {
    qint64 animalId{};
    QString text;

    QJsonObject toJson() const;
    static ReviewCreateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models