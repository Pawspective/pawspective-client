#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QString>

namespace pawspective::models {

struct PostDTO {
    qint64 id{};
    qint64 organizationId{};
    QString text;
    QDateTime createdAt{};

    QJsonObject toJson() const;
    static PostDTO fromJson(const QJsonObject& json);
};

struct PostListDTO {
    QList<PostDTO> items;
    int page{};
    int limit{};
    qint64 totalCount{};
    qint64 totalPages{};

    static PostListDTO fromJson(const QJsonObject& json);
};

}  // namespace pawspective::models
