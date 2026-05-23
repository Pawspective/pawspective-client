#include "models/review_create_dto.hpp"

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject ReviewCreateDTO::toJson() const {
    QJsonObject json;

    json["animal_id"] = static_cast<qint64>(animalId);
    json["text"] = text;

    return json;
}

ReviewCreateDTO ReviewCreateDTO::fromJson(const QJsonObject& json) {
    ReviewCreateDTO dto;

    dto.animalId = static_cast<quint64>(utils::json::getRequiredInt64(json, "animal_id"));
    dto.text = utils::json::getRequiredString(json, "text");

    return dto;
}

}  // namespace pawspective::models
