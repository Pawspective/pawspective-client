#include "models/post_create_dto.hpp"

#include <QJsonArray>

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject PostCreateDTO::toJson() const {
    QJsonObject json;

    json["text"] = text;

    QJsonArray photosArray;
    for (const auto& photo : photos) {
        photosArray.append(photo);
    }
    json["photos"] = photosArray;

    return json;
}

PostCreateDTO PostCreateDTO::fromJson(const QJsonObject& json) {
    PostCreateDTO dto;

    dto.text = utils::json::getRequiredString(json, "text");

    return dto;
}

}  // namespace pawspective::models
