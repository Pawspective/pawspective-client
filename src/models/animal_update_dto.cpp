#include "../include/models/animal_update_dto.hpp"
#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject AnimalUpdateDTO::toJson() const {
    QJsonObject json;

    if (name.has_value()) {
        json["name"] = name.value();
    }
    if (breedId.has_value()) {
        json["breed_id"] = breedId.value();
    }
    if (size.has_value()) {
        json["size"] = toApiString(size.value());
    }
    if (gender.has_value()) {
        json["gender"] = toApiString(gender.value());
    }
    if (careLevel.has_value()) {
        json["care_level"] = toApiString(careLevel.value());
    }
    if (color.has_value()) {
        json["color"] = toApiString(color.value());
    }
    if (goodWith.has_value()) {
        json["good_with"] = toApiString(goodWith.value());
    }
    if (age.has_value()) {
        json["age"] = age.value();
    }
    if (description.has_value()) {
        json["description"] = description.value();
    }
    if (status.has_value()) {
        json["status"] = toApiString(status.value());
    }

    return json;
}

AnimalUpdateDTO AnimalUpdateDTO::fromJson(const QJsonObject& json) {
    AnimalUpdateDTO dto;
    dto.name = utils::json::getOptionalString(json, "name");
    dto.breedId = utils::json::getOptionalInt64(json, "breed_id");
    dto.age = utils::json::getOptionalInt32(json, "age");
    dto.description = utils::json::getOptionalString(json, "description");
    dto.careLevel = utils::json::getOptionalEnum<CareLevel>(json, "care_level", careLevelFromApi);
    dto.size = utils::json::getOptionalEnum<AnimalSize>(json, "size", animalSizeFromApi);
    dto.gender = utils::json::getOptionalEnum<AnimalGender>(json, "gender", animalGenderFromApi);
    dto.color = utils::json::getOptionalEnum<AnimalColor>(json, "color", animalColorFromApi);
    dto.goodWith = utils::json::getOptionalEnum<GoodWith>(json, "good_with", goodWithFromApi);
    dto.status = utils::json::getOptionalEnum<AnimalStatus>(json, "status", animalStatusFromApi);
    return dto;
}

}  // namespace pawspective::models
