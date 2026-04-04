#pragma once

#include <QVariantList>
#include <functional>
#include <optional>

#include "base.hpp"
#include "models/animal_filter_dto.hpp"
#include "models/animal_register_dto.hpp"
#include "models/breed_dto.hpp"
#include "services/animal_service.hpp"
#include "services/breed_service.hpp"

namespace pawspective::viewmodels {

class CreateAnimalViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString animalType READ animalType WRITE setAnimalType NOTIFY animalTypeChanged)
    Q_PROPERTY(qint64 breedId READ breedId WRITE setBreedId NOTIFY breedIdChanged)
    Q_PROPERTY(QString size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QString gender READ gender WRITE setGender NOTIFY genderChanged)
    Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)
    Q_PROPERTY(QString careLevel READ careLevel WRITE setCareLevel NOTIFY careLevelChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QString goodWith READ goodWith WRITE setGoodWith NOTIFY goodWithChanged)
    Q_PROPERTY(bool isBreedEnabled READ isBreedEnabled NOTIFY isBreedEnabledChanged)

    Q_PROPERTY(QVariantList animalTypes READ animalTypes NOTIFY animalTypesChanged)
    Q_PROPERTY(QVariantList breeds READ breeds NOTIFY breedsChanged)
    Q_PROPERTY(QVariantList sizes READ sizes NOTIFY sizesChanged)
    Q_PROPERTY(QVariantList genders READ genders NOTIFY gendersChanged)
    Q_PROPERTY(QVariantList careLevels READ careLevels NOTIFY careLevelsChanged)
    Q_PROPERTY(QVariantList colors READ colors NOTIFY colorsChanged)
    Q_PROPERTY(QVariantList goodWiths READ goodWiths NOTIFY goodWithsChanged)

public:
    explicit CreateAnimalViewModel(
        services::AnimalService& animalService,
        services::BreedService& breedService,
        QObject* parent = nullptr
    );

    const QString& name() const { return m_registerDto.name; }
    QString description() const { return m_registerDto.description.value_or(""); }
    QString animalType() const { return m_animalType.has_value() ? models::toApiString(m_animalType.value()) : ""; }
    qint64 breedId() const { return m_registerDto.breedId; }
    QString size() const { return m_size.has_value() ? models::toApiString(m_size.value()) : ""; }
    QString gender() const { return m_gender.has_value() ? models::toApiString(m_gender.value()) : ""; }
    int age() const { return m_registerDto.age; }
    QString careLevel() const { return m_careLevel.has_value() ? models::toApiString(m_careLevel.value()) : ""; }
    QString color() const { return m_color.has_value() ? models::toApiString(m_color.value()) : ""; }
    QString goodWith() const { return m_goodWith.has_value() ? models::toApiString(m_goodWith.value()) : ""; }
    bool isBreedEnabled() const { return m_animalType.has_value() && !m_isLoadingBreeds; }

    QVariantList animalTypes() const { return toVariantList(m_filterDto.animalTypes, &models::toApiString); }
    QVariantList breeds() const { return m_breedsList; }
    QVariantList sizes() const { return toVariantList(m_filterDto.sizes, &models::toApiString); }
    QVariantList genders() const { return toVariantList(m_filterDto.genders, &models::toApiString); }
    QVariantList careLevels() const { return toVariantList(m_filterDto.careLevels, &models::toApiString); }
    QVariantList colors() const { return toVariantList(m_filterDto.colors, &models::toApiString); }
    QVariantList goodWiths() const { return toVariantList(m_filterDto.goodWiths, &models::toApiString); }

    void setName(const QString& value) {
        updateProperty(m_registerDto.name, value, [this] { emit nameChanged(); });
    }

    void setDescription(const QString& value) {
        auto newDescription = value.isEmpty() ? std::optional<QString>{} : std::optional<QString>{value};
        updateProperty(m_registerDto.description, newDescription, [this] { emit descriptionChanged(); });
    }

    void setAnimalType(const QString& value);
    void setBreedId(qint64 value) {
        updateProperty(m_registerDto.breedId, value, [this] { emit breedIdChanged(); });
    }

    void setSize(const QString& value) {
        if (value.isEmpty()) {
            if (m_size.has_value()) {
                m_size = std::nullopt;
                emit sizeChanged();
            }
        } else {
            auto newSize = models::animalSizeFromApi(value.toLower());
            if (!m_size.has_value() || m_size.value() != newSize) {
                m_size = newSize;
                emit sizeChanged();
            }
        }
    }

    void setGender(const QString& value) {
        if (value.isEmpty()) {
            if (m_gender.has_value()) {
                m_gender = std::nullopt;
                emit genderChanged();
            }
        } else {
            auto newGender = models::animalGenderFromApi(value.toLower());
            if (!m_gender.has_value() || m_gender.value() != newGender) {
                m_gender = newGender;
                emit genderChanged();
            }
        }
    }

    void setAge(int value) {
        updateProperty(m_registerDto.age, value, [this] { emit ageChanged(); });
    }

    void setCareLevel(const QString& value) {
        if (value.isEmpty()) {
            if (m_careLevel.has_value()) {
                m_careLevel = std::nullopt;
                emit careLevelChanged();
            }
        } else {
            auto newCareLevel = models::careLevelFromApi(value.toLower());
            if (!m_careLevel.has_value() || m_careLevel.value() != newCareLevel) {
                m_careLevel = newCareLevel;
                emit careLevelChanged();
            }
        }
    }

    void setColor(const QString& value) {
        if (value.isEmpty()) {
            if (m_color.has_value()) {
                m_color = std::nullopt;
                emit colorChanged();
            }
        } else {
            auto newColor = models::animalColorFromApi(value.toLower());
            if (!m_color.has_value() || m_color.value() != newColor) {
                m_color = newColor;
                emit colorChanged();
            }
        }
    }

    void setGoodWith(const QString& value) {
        if (value.isEmpty()) {
            if (m_goodWith.has_value()) {
                m_goodWith = std::nullopt;
                emit goodWithChanged();
            }
        } else {
            auto newGoodWith = models::goodWithFromApi(value.toLower());
            if (!m_goodWith.has_value() || m_goodWith.value() != newGoodWith) {
                m_goodWith = newGoodWith;
                emit goodWithChanged();
            }
        }
    }

    Q_INVOKABLE void createAnimal();
    Q_INVOKABLE void loadFilters();
    Q_INVOKABLE void setOrganizationId(qint64 organizationId) {
        m_organizationId = organizationId;
        m_registerDto.organizationId = organizationId;
    }
    Q_INVOKABLE void setBreedId(const QString& value) {
        bool ok = false;
        qint64 id = value.toLongLong(&ok);
        if (ok) {
            setBreedId(id);
        }
    }

    void initialize() override;
    void cleanup() override {}

signals:
    void nameChanged();
    void descriptionChanged();
    void animalTypeChanged();
    void breedIdChanged();
    void sizeChanged();
    void genderChanged();
    void ageChanged();
    void careLevelChanged();
    void colorChanged();
    void goodWithChanged();
    void isBreedEnabledChanged();

    void animalTypesChanged();
    void breedsChanged();
    void sizesChanged();
    void gendersChanged();
    void careLevelsChanged();
    void colorsChanged();
    void goodWithsChanged();

    void creationFinished(bool success);

private:
    void setupConnections();
    void onFiltersLoaded(const models::AnimalFilterDTO& filters);
    void onBreedsLoaded(const QList<models::BreedDTO>& breeds);
    void onBreedsLoadFailed(QSharedPointer<services::BaseError> error);
    void onAnimalCreated();
    void onError(QSharedPointer<services::BaseError> error);
    void loadBreedsForType(models::AnimalType type);
    void updateBreedsList(const QList<models::BreedDTO>& breeds);
    bool validateRequiredFields();
    void fillRegisterDto();

    template <typename T>
    QVariantList toVariantList(const std::optional<QVector<T>>& vec) const {
        QVariantList list;

        QVariantMap emptyEntry;
        emptyEntry["text"] = "";
        emptyEntry["value"] = "";
        list.append(emptyEntry);

        if (vec.has_value()) {
            for (const auto& item : *vec) {
                QVariantMap entry;
                entry["text"] = QString::number(item);
                entry["value"] = QString::number(item);
                list.append(entry);
            }
        }
        return list;
    }

    template <typename T>
    QVariantList toVariantList(const std::optional<QVector<T>>& vec, QString (*converter)(T)) const {
        QVariantList list;

        QVariantMap emptyEntry;
        emptyEntry["text"] = "";
        emptyEntry["value"] = "";
        list.append(emptyEntry);

        if (vec.has_value()) {
            for (const auto& item : *vec) {
                QVariantMap entry;
                entry["text"] = converter(item);
                entry["value"] = converter(item);
                list.append(entry);
            }
        }
        return list;
    }

    services::AnimalService& m_animalService;
    services::BreedService& m_breedService;
    qint64 m_organizationId = 0;

    models::AnimalRegisterDTO m_registerDto;
    models::AnimalFilterDTO m_filterDto;

    std::optional<models::AnimalType> m_animalType;
    std::optional<models::AnimalSize> m_size;
    std::optional<models::AnimalGender> m_gender;
    std::optional<models::CareLevel> m_careLevel;
    std::optional<models::AnimalColor> m_color;
    std::optional<models::GoodWith> m_goodWith;

    QVariantList m_breedsList;
    bool m_isLoadingBreeds = false;
};

}  // namespace pawspective::viewmodels