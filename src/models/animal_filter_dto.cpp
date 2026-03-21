#include "models/animal_filter_dto.hpp"
#include <qjsonarray.h>
#include "utils/json.hpp"

namespace pawspective::models {

QJsonObject AnimalFilterDTO::toJson() const {
    QJsonObject json;

    if (breeds.has_value()) {
        QJsonArray breedsArray;
        for (const auto& breed : breeds.value()) {
            breedsArray.append(breed);
        }
        json["breeds"] = breedsArray;
    }
    if (animalTypes.has_value()) {
        QJsonArray animalTypesArray;
        for (const auto& animalType : animalTypes.value()) {
            animalTypesArray.append(toApiString(animalType));
        }
        json["animal_types"] = animalTypesArray;
    }
    if (sizes.has_value()) {
        QJsonArray sizesArray;
        for (const auto& size : sizes.value()) {
            sizesArray.append(toApiString(size));
        }
        json["sizes"] = sizesArray;
    }
    if (genders.has_value()) {
        QJsonArray gendersArray;
        for (const auto& gender : genders.value()) {
            gendersArray.append(toApiString(gender));
        }
        json["genders"] = gendersArray;
    }
    if (careLevels.has_value()) {
        QJsonArray careLevelsArray;
        for (const auto& careLevel : careLevels.value()) {
            careLevelsArray.append(toApiString(careLevel));
        }
        json["care_levels"] = careLevelsArray;
    }
    if (colors.has_value()) {
        QJsonArray colorsArray;
        for (const auto& color : colors.value()) {
            colorsArray.append(toApiString(color));
        }
        json["colors"] = colorsArray;
    }
    if (goodWiths.has_value()) {
        QJsonArray goodWithsArray;
        for (const auto& goodWith : goodWiths.value()) {
            goodWithsArray.append(toApiString(goodWith));
        }
        json["good_withs"] = goodWithsArray;
    }
    if (ageLte.has_value()) {
        json["age_lte"] = ageLte.value();
    }
    if (ageGte.has_value()) {
        json["age_gte"] = ageGte.value();
    }

    return json;
}

AnimalFilterDTO AnimalFilterDTO::fromJson(const QJsonObject& json) {
    AnimalFilterDTO dto;

    if (json.contains("breeds") && json["breeds"].isArray()) {
        QJsonArray breedsArray = json["breeds"].toArray();
        QVector<int64_t> breedsVec;
        for (const auto& breedId : breedsArray) {
            if (breedId.isDouble()) {
                breedsVec.append(breedId.toInteger());
            } else {
                throw std::invalid_argument("Invalid value in breeds array");
            }
        }
        dto.breeds = breedsVec;
    }

    dto.animalTypes = utils::json::getOptionalEnumArray<AnimalType>(json, "animal_types", animalTypeFromApi);
    dto.sizes = utils::json::getOptionalEnumArray<AnimalSize>(json, "sizes", animalSizeFromApi);
    dto.genders = utils::json::getOptionalEnumArray<AnimalGender>(json, "genders", animalGenderFromApi);
    dto.careLevels = utils::json::getOptionalEnumArray<CareLevel>(json, "care_levels", careLevelFromApi);
    dto.colors = utils::json::getOptionalEnumArray<AnimalColor>(json, "colors", animalColorFromApi);
    dto.goodWiths = utils::json::getOptionalEnumArray<GoodWith>(json, "good_withs", goodWithFromApi);
    dto.ageLte = utils::json::getOptionalInt32(json, "age_lte");
    dto.ageGte = utils::json::getOptionalInt32(json, "age_gte");

    return dto;
}

}  // namespace pawspective::models
