#include "models/review_update_dto.hpp"

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject ReviewUpdateDTO::toJson() const {
    QJsonObject json;

    if (text.has_value()) {
        json["text"] = text.value();
    }

    return json;
}

ReviewUpdateDTO ReviewUpdateDTO::fromJson(const QJsonObject& json) {
    ReviewUpdateDTO dto;

    dto.text = utils::json::getOptionalString(json, "text");

    return dto;
}

}  // namespace pawspective::models
