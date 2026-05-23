#include "models/adopt_request_dto.hpp"

#include <QJsonArray>

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject AdoptRequestDTO::toJson() const {
    QJsonObject json;
    json["id"] = id;
    json["email"] = email;
    json["animal"] = animal.toJson();
    return json;
}

AdoptRequestDTO AdoptRequestDTO::fromJson(const QJsonObject& json) {
    AdoptRequestDTO dto;
    dto.id = utils::json::getRequiredInt64(json, "id");
    dto.email = utils::json::getRequiredString(json, "email");
    dto.animal = AnimalDTO::fromJson(utils::json::getRequiredObject(json, "animal"));
    return dto;
}

AdoptRequestListDTO AdoptRequestListDTO::fromJson(const QJsonObject& json) {
    AdoptRequestListDTO dto;
    dto.page = utils::json::getRequiredInt32(json, "page");
    dto.limit = utils::json::getRequiredInt32(json, "limit");
    dto.totalCount = utils::json::getRequiredInt64(json, "total_count");
    dto.totalPages = utils::json::getRequiredInt64(json, "total_pages");

    const QJsonArray items = json["items"].toArray();
    for (const auto& item : items) {
        dto.items.append(AdoptRequestDTO::fromJson(item.toObject()));
    }
    return dto;
}

}  // namespace pawspective::models
