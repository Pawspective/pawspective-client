#include "models/breed_dto.hpp"
#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject BreedDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["animal_type"] = toApiString(animalType);
    json["name"] = name;

    return json;
}

BreedDTO BreedDTO::fromJson(const QJsonObject& json) {
    BreedDTO dto;
    dto.id = utils::json::getRequiredInt64(json, "id");
    dto.animalType = animalTypeFromApi(pawspective::utils::json::getRequiredString(json, "animal_type"));
    auto nameOpt = utils::json::getOptionalString(json, "name");
    dto.name = nameOpt.value_or("");
    return dto;
}

}  // namespace pawspective::models
