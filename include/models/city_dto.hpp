#pragma once

#include <QJsonObject>
#include <QString>

namespace pawspective::models {

struct CityDTO {
    qint64 id = 0;
    QString name;

    QJsonObject toJson() const;
    static CityDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
