#pragma once

#include <QJsonObject>
#include <QString>

namespace pawspective::models {

struct PostCreateDTO {
    QString text;

    QJsonObject toJson() const;
    static PostCreateDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models