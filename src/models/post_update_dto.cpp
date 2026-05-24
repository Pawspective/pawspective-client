#include "models/post_update_dto.hpp"

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject PostUpdateDTO::toJson() const {
    QJsonObject json;

    if (text.has_value()) {
        json["text"] = text.value();
    }

    return json;
}

PostUpdateDTO PostUpdateDTO::fromJson(const QJsonObject& json) {
    PostUpdateDTO dto;

    dto.text = utils::json::getOptionalString(json, "text");

    return dto;
}

}  // namespace pawspective::models
