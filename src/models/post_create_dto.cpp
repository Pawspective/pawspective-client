#include "models/post_create_dto.hpp"

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject PostCreateDTO::toJson() const {
    QJsonObject json;

    json["text"] = text;

    return json;
}

PostCreateDTO PostCreateDTO::fromJson(const QJsonObject& json) {
    PostCreateDTO dto;

    dto.text = utils::json::getRequiredString(json, "text");

    return dto;
}

}  // namespace pawspective::models
