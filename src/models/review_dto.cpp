#include "models/review_dto.hpp"

#include <QDateTime>
#include <QJsonArray>


#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject ReviewDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["animal_id"] = animalId;
    json["animal_name"] = animalName;
    json["text"] = text;
    json["created_at"] = createdAt.toString(Qt::ISODateWithMs);
    json["can_edit"] = canEdit;

    return json;
}

ReviewDTO ReviewDTO::fromJson(const QJsonObject& json) {
    ReviewDTO dto;

    dto.id = utils::json::getRequiredInt64(json, "id");
    dto.animalId = utils::json::getRequiredInt64(json, "animal_id");
    dto.animalName = utils::json::getRequiredString(json, "animal_name");
    dto.text = utils::json::getRequiredString(json, "text");
    dto.createdAt = utils::json::getRequiredDateTime(json, "created_at");
    dto.canEdit = utils::json::getRequiredBool(json, "can_edit");

    return dto;
}

ReviewListDTO ReviewListDTO::fromJson(const QJsonObject& json) {
    ReviewListDTO dto;

    dto.page = utils::json::getRequiredInt32(json, "page");
    dto.limit = utils::json::getRequiredInt32(json, "limit");
    dto.totalCount = utils::json::getRequiredInt64(json, "total_count");
    dto.totalPages = utils::json::getRequiredInt64(json, "total_pages");

    const QJsonArray items = json["items"].toArray();
    for (const auto& item : items) {
        dto.items.append(ReviewDTO::fromJson(item.toObject()));
    }

    return dto;
}

}  // namespace pawspective::models
