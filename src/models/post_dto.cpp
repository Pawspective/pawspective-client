#include "models/post_dto.hpp"

#include <QDateTime>
#include <QJsonArray>

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject PostDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["organization_id"] = organizationId;
    json["text"] = text;
    json["created_at"] = createdAt.toString(Qt::ISODateWithMs);

    QJsonArray photosArray;
    for (const auto& photo : photos) {
        photosArray.append(photo);
    }
    json["photos"] = photosArray;

    return json;
}

PostDTO PostDTO::fromJson(const QJsonObject& json) {
    PostDTO dto;

    dto.id = utils::json::getRequiredInt64(json, "id");
    dto.organizationId = utils::json::getRequiredInt64(json, "organization_id");
    dto.text = utils::json::getRequiredString(json, "text");
    dto.createdAt = utils::json::getRequiredDateTime(json, "created_at");

    if (json.contains("photos") && json["photos"].isArray()) {
        const QJsonArray photosArr = json["photos"].toArray();
        for (const auto& p : photosArr) {
            if (p.isString()) {
                dto.photos.append(p.toString());
            }
        }
    }

    return dto;
}

PostListDTO PostListDTO::fromJson(const QJsonObject& json) {
    PostListDTO dto;

    dto.page = utils::json::getRequiredInt32(json, "page");
    dto.limit = utils::json::getRequiredInt32(json, "limit");
    dto.totalCount = utils::json::getRequiredInt64(json, "total_count");
    dto.totalPages = utils::json::getRequiredInt64(json, "total_pages");

    const QJsonArray items = json["items"].toArray();
    for (const auto& item : items) {
        dto.items.append(PostDTO::fromJson(item.toObject()));
    }

    return dto;
}

}  // namespace pawspective::models
