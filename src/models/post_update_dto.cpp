#include "models/post_update_dto.hpp"

#include <QJsonArray>

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject PostUpdateDTO::toJson() const {
    QJsonObject json;

    if (text.has_value()) {
        json["text"] = text.value();
    }

    if (photos.has_value()) {
        QJsonArray photosArray;
        for (const auto& photo : photos.value()) {
            photosArray.append(photo);
        }
        json["photos"] = photosArray;
    }

    return json;
}

PostUpdateDTO PostUpdateDTO::fromJson(const QJsonObject& json) {
    PostUpdateDTO dto;

    dto.text = utils::json::getOptionalString(json, "text");

    return dto;
}

}  // namespace pawspective::models
