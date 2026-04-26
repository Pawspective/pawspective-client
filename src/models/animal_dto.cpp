#include "../include/models/animal_dto.hpp"

#include <QJsonArray>

#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject AnimalDTO::toJson() const {
    QJsonObject json;

    json["id"] = id;
    json["organization_id"] = organizationId;
    json["name"] = name;
    json["breed"] = breed.toJson();
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

AnimalDTO AnimalDTO::fromJson(const QJsonObject& json) {
    AnimalDTO dto;
    dto.id = pawspective::utils::json::getRequiredInt64(json, "id");
    dto.organizationId = pawspective::utils::json::getRequiredInt64(json, "organization_id");
    dto.name = pawspective::utils::json::getRequiredString(json, "name");
    dto.breed = BreedDTO::fromJson(pawspective::utils::json::getRequiredObject(json, "breed"));
    dto.size = animalSizeFromApi(pawspective::utils::json::getRequiredString(json, "size"));
    dto.gender = animalGenderFromApi(pawspective::utils::json::getRequiredString(json, "gender"));
    dto.careLevel = careLevelFromApi(pawspective::utils::json::getRequiredString(json, "care_level"));
    dto.color = animalColorFromApi(pawspective::utils::json::getRequiredString(json, "color"));
    dto.goodWith = goodWithFromApi(pawspective::utils::json::getRequiredString(json, "good_with"));
    dto.age = pawspective::utils::json::getRequiredInt32(json, "age");
    dto.description = pawspective::utils::json::getOptionalString(json, "description");
    dto.status = animalStatusFromApi(pawspective::utils::json::getRequiredString(json, "status"));
    return dto;
}

AnimalListDTO AnimalListDTO::fromJson(const QJsonObject& json) {
    AnimalListDTO dto;
    dto.page = pawspective::utils::json::getRequiredInt32(json, "page");
    dto.limit = pawspective::utils::json::getRequiredInt32(json, "limit");
    dto.totalCount = pawspective::utils::json::getRequiredInt64(json, "total_count");
    dto.totalPages = pawspective::utils::json::getRequiredInt64(json, "total_pages");

    const QJsonArray items = json["items"].toArray();
    for (const auto& item : items) {
        dto.items.append(AnimalDTO::fromJson(item.toObject()));
    }
    return dto;
}

}  // namespace pawspective::models
