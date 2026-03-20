#include "../include/models/animal_register_dto.hpp"
#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject AnimalRegisterDTO::toJson() const {
    QJsonObject json;

    json["organization_id"] = organizationId;
    json["name"] = name;
    json["breed_id"] = breedId;
    json["size"] = toApiString(size);
    json["gender"] = toApiString(gender);
    json["care_level"] = toApiString(careLevel);
    json["color"] = toApiString(color);
    json["good_with"] = toApiString(goodWith);
    json["age"] = age;
    if (description.has_value()) {
        json["description"] = description.value();
    }
    json["status"] = toApiString(status);

    return json;
}

AnimalRegisterDTO AnimalRegisterDTO::fromJson(const QJsonObject& json) {
    AnimalRegisterDTO dto;
    dto.organizationId = pawspective::utils::json::getRequiredInt64(json, "organization_id");
    dto.name = pawspective::utils::json::getRequiredString(json, "name");
    dto.breedId = pawspective::utils::json::getRequiredInt64(json, "breed_id");
    dto.size = animalSizeFromApi(utils::json::getRequiredString(json, "size"));
    dto.gender = animalGenderFromApi(utils::json::getRequiredString(json, "gender"));
    dto.careLevel = careLevelFromApi(utils::json::getRequiredString(json, "care_level"));
    dto.color = animalColorFromApi(utils::json::getRequiredString(json, "color"));
    dto.goodWith = goodWithFromApi(utils::json::getRequiredString(json, "good_with"));
    dto.age = utils::json::getRequiredInt32(json, "age");
    dto.description = utils::json::getOptionalString(json, "description");  // Updated to use getOptionalString
    dto.status = animalStatusFromApi(utils::json::getRequiredString(json, "status"));
    return dto;
}

}  // namespace pawspective::models
