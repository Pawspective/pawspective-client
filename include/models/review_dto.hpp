#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QString>

namespace pawspective::models {

struct ReviewDTO {
    qint64 id{};
    qint64 animalId{};
    QString animalName;
    QString text;
    QDateTime createdAt{};
    bool canEdit = false;

    QJsonObject toJson() const;
    static ReviewDTO fromJson(const QJsonObject& json);
};

struct ReviewListDTO {
    QList<ReviewDTO> items;
    int page{};
    int limit{};
    qint64 totalCount{};
    qint64 totalPages{};

    static ReviewListDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
