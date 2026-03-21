#include "../include/models/animal_enums.hpp"
#include <stdexcept>

namespace pawspective::models {

QString toApiString(AnimalSize value) {
    switch (value) {
        case AnimalSize::Small:
            return "small";
        case AnimalSize::Medium:
            return "medium";
        case AnimalSize::Large:
            return "large";
        default:
            throw std::invalid_argument("Invalid AnimalSize enum value");
    }
}

AnimalSize animalSizeFromApi(const QString& value) {
    if (value == "small") {
        return AnimalSize::Small;
    }
    if (value == "medium") {
        return AnimalSize::Medium;
    }
    if (value == "large") {
        return AnimalSize::Large;
    }
    throw std::invalid_argument("Invalid AnimalSize: " + value.toStdString());
}

QString toApiString(AnimalGender value) {
    switch (value) {
        case AnimalGender::Male:
            return "male";
        case AnimalGender::Female:
            return "female";
        case AnimalGender::Unknown:
            return "unknown";
        default:
            throw std::invalid_argument("Invalid AnimalGender enum value");
    }
}

AnimalGender animalGenderFromApi(const QString& value) {
    if (value == "male") {
        return AnimalGender::Male;
    }
    if (value == "female") {
        return AnimalGender::Female;
    }
    if (value == "unknown") {
        return AnimalGender::Unknown;
    }
    throw std::invalid_argument("Invalid AnimalGender: " + value.toStdString());
}

QString toApiString(CareLevel value) {
    switch (value) {
        case CareLevel::Easy:
            return "easy";
        case CareLevel::Moderate:
            return "moderate";
        case CareLevel::Difficult:
            return "difficult";
        case CareLevel::SpecialNeeds:
            return "special_needs";
        default:
            throw std::invalid_argument("Invalid CareLevel enum value");
    }
}

CareLevel careLevelFromApi(const QString& value) {
    if (value == "easy") {
        return CareLevel::Easy;
    }
    if (value == "moderate") {
        return CareLevel::Moderate;
    }
    if (value == "difficult") {
        return CareLevel::Difficult;
    }
    if (value == "special_needs") {
        return CareLevel::SpecialNeeds;
    }
    throw std::invalid_argument("Invalid CareLevel: " + value.toStdString());
}

QString toApiString(GoodWith value) {
    switch (value) {
        case GoodWith::Dogs:
            return "dogs";
        case GoodWith::Cats:
            return "cats";
        case GoodWith::Children:
            return "children";
        case GoodWith::Elderly:
            return "elderly";
        default:
            throw std::invalid_argument("Invalid GoodWith enum value");
    }
}

GoodWith goodWithFromApi(const QString& value) {
    if (value == "dogs") {
        return GoodWith::Dogs;
    }
    if (value == "cats") {
        return GoodWith::Cats;
    }
    if (value == "children") {
        return GoodWith::Children;
    }
    if (value == "elderly") {
        return GoodWith::Elderly;
    }
    throw std::invalid_argument("Invalid GoodWith: " + value.toStdString());
}

QString toApiString(AnimalColor value) {
    switch (value) {
        case AnimalColor::Black:
            return "black";
        case AnimalColor::White:
            return "white";
        case AnimalColor::Brown:
            return "brown";
        case AnimalColor::Grey:
            return "grey";
        case AnimalColor::Orange:
            return "orange";
        case AnimalColor::Cream:
            return "cream";
        case AnimalColor::Tan:
            return "tan";
        case AnimalColor::Golden:
            return "golden";
        case AnimalColor::Spotted:
            return "spotted";
        case AnimalColor::Striped:
            return "striped";
        case AnimalColor::Brindle:
            return "brindle";
        case AnimalColor::Mixed:
            return "mixed";
        default:
            throw std::invalid_argument("Invalid AnimalColor enum value");
    }
}

AnimalColor animalColorFromApi(const QString& value) {
    if (value == "black") {
        return AnimalColor::Black;
    }
    if (value == "white") {
        return AnimalColor::White;
    }
    if (value == "brown") {
        return AnimalColor::Brown;
    }
    if (value == "grey") {
        return AnimalColor::Grey;
    }
    if (value == "orange") {
        return AnimalColor::Orange;
    }
    if (value == "cream") {
        return AnimalColor::Cream;
    }
    if (value == "tan") {
        return AnimalColor::Tan;
    }
    if (value == "golden") {
        return AnimalColor::Golden;
    }
    if (value == "spotted") {
        return AnimalColor::Spotted;
    }
    if (value == "striped") {
        return AnimalColor::Striped;
    }
    if (value == "brindle") {
        return AnimalColor::Brindle;
    }
    if (value == "mixed") {
        return AnimalColor::Mixed;
    }
    throw std::invalid_argument("Invalid AnimalColor: " + value.toStdString());
}

QString toApiString(AnimalStatus value) {
    switch (value) {
        case AnimalStatus::Available:
            return "available";
        case AnimalStatus::Adopted:
            return "adopted";
        case AnimalStatus::Unavailable:
            return "unavailable";
        default:
            throw std::invalid_argument("Invalid AnimalStatus enum value");
    }
}

AnimalStatus animalStatusFromApi(const QString& value) {
    if (value == "available") {
        return AnimalStatus::Available;
    }
    if (value == "adopted") {
        return AnimalStatus::Adopted;
    }
    if (value == "unavailable") {
        return AnimalStatus::Unavailable;
    }
    throw std::invalid_argument("Invalid AnimalStatus: " + value.toStdString());
}

QString toApiString(AnimalType value) {
    switch (value) {
        case AnimalType::Dog:
            return "dog";
        case AnimalType::Cat:
            return "cat";
        case AnimalType::Other:
            return "other";
        default:
            throw std::invalid_argument("Invalid AnimalType enum value");
    }
}

AnimalType animalTypeFromApi(const QString& value) {
    if (value == "dog") {
        return AnimalType::Dog;
    }
    if (value == "cat") {
        return AnimalType::Cat;
    }
    if (value == "other") {
        return AnimalType::Other;
    }
    throw std::invalid_argument("Invalid AnimalType: " + value.toStdString());
}

}  // namespace pawspective::models
