#pragma once

#include <QString>
#include <cstdint>

namespace pawspective::models {

enum class AnimalSize : std::uint8_t { Small, Medium, Large };

enum class AnimalGender : std::uint8_t { Male, Female, Unknown };

enum class CareLevel : std::uint8_t { Easy, Moderate, Difficult, SpecialNeeds };

enum class GoodWith : std::uint8_t { Dogs, Cats, Children, Elderly };

enum class AnimalColor : std::uint8_t {
    Black,
    White,
    Brown,
    Grey,
    Orange,
    Cream,
    Tan,
    Golden,
    Spotted,
    Striped,
    Brindle,
    Mixed
};

enum class AnimalStatus : std::uint8_t { Available, Adopted, Unavailable };

enum class AnimalType : std::uint8_t { Dog, Cat, Other };

QString toApiString(AnimalSize value);
QString toApiString(AnimalGender value);
QString toApiString(CareLevel value);
QString toApiString(GoodWith value);
QString toApiString(AnimalColor value);
QString toApiString(AnimalStatus value);
QString toApiString(AnimalType value);

AnimalSize animalSizeFromApi(const QString& value);
AnimalGender animalGenderFromApi(const QString& value);
CareLevel careLevelFromApi(const QString& value);
GoodWith goodWithFromApi(const QString& value);
AnimalColor animalColorFromApi(const QString& value);
AnimalStatus animalStatusFromApi(const QString& value);
AnimalType animalTypeFromApi(const QString& value);

}  // namespace pawspective::models
