#pragma once

#include <QJsonObject>
#include <QString>
#include <QStringList>

namespace pawspective::models {

struct PostCreateDTO {
    QString text;
    QStringList photos;

    QJsonObject toJson() const;
    static PostCreateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models