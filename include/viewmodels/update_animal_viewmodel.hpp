#pragma once

#include <QVariantList>
#include <optional>

#include "base.hpp"
#include "models/animal_filter_dto.hpp"
#include "models/animal_update_dto.hpp"
#include "models/breed_dto.hpp"
#include "services/animal_service.hpp"
#include "services/breed_service.hpp"

namespace pawspective::viewmodels {

class UpdateAnimalViewModel : public BaseViewModel {
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
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool isBreedEnabled READ isBreedEnabled NOTIFY isBreedEnabledChanged)
    Q_PROPERTY(bool isDirty READ isDirty NOTIFY dirtyChanged)

    Q_PROPERTY(QVariantList animalTypes READ animalTypes NOTIFY animalTypesChanged)
    Q_PROPERTY(QVariantList breeds READ breeds NOTIFY breedsChanged)
    Q_PROPERTY(QVariantList sizes READ sizes NOTIFY sizesChanged)
    Q_PROPERTY(QVariantList genders READ genders NOTIFY gendersChanged)
    Q_PROPERTY(QVariantList careLevels READ careLevels NOTIFY careLevelsChanged)
    Q_PROPERTY(QVariantList colors READ colors NOTIFY colorsChanged)
    Q_PROPERTY(QVariantList goodWiths READ goodWiths NOTIFY goodWithsChanged)
    Q_PROPERTY(QVariantList statuses READ statuses NOTIFY statusesChanged)

public:
    explicit UpdateAnimalViewModel(
        services::AnimalService& animalService,
        services::BreedService& breedService,
        QObject* parent = nullptr
    );

    QString name() const { return m_changes.name.value_or(m_originalData.name); }
    QString description() const { return m_changes.description.value_or(m_originalData.description.value_or("")); }
    QString animalType() const;
    qint64 breedId() const;
    QString size() const;
    QString gender() const;
    int age() const;
    QString careLevel() const;
    QString color() const;
    QString goodWith() const;
    QString status() const;
    bool isBreedEnabled() const;
    bool isDirty() const { return m_isDirty; }

    QVariantList animalTypes() const { return toVariantList(m_filterDto.animalTypes, &models::toApiString); }
    QVariantList breeds() const { return m_breedsList; }
    QVariantList sizes() const { return toVariantList(m_filterDto.sizes, &models::toApiString); }
    QVariantList genders() const { return toVariantList(m_filterDto.genders, &models::toApiString); }
    QVariantList careLevels() const { return toVariantList(m_filterDto.careLevels, &models::toApiString); }
    QVariantList colors() const { return toVariantList(m_filterDto.colors, &models::toApiString); }
    QVariantList goodWiths() const { return toVariantList(m_filterDto.goodWiths, &models::toApiString); }
    QVariantList statuses() const;

    void setName(const QString& value);
    void setDescription(const QString& value);
    void setAnimalType(const QString& value);
    void setBreedId(qint64 value);
    void setSize(const QString& value);
    void setGender(const QString& value);
    void setAge(int value);
    void setCareLevel(const QString& value);
    void setColor(const QString& value);
    void setGoodWith(const QString& value);
    void setStatus(const QString& value);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void discardChanges();
    Q_INVOKABLE void setAnimalId(qint64 animalId) { m_animalId = animalId; }

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
    void statusChanged();
    void isBreedEnabledChanged();
    void dirtyChanged();

    void animalTypesChanged();
    void breedsChanged();
    void sizesChanged();
    void gendersChanged();
    void careLevelsChanged();
    void colorsChanged();
    void goodWithsChanged();
    void statusesChanged();

    void loadCompleted();
    void loadFailed(const QString& errorMessage);
    void saveCompleted();
    void saveFailed(const QString& errorMessage);

private slots:
    void handleGetSuccess(const models::AnimalDTO& animal);
    void handleGetFailed(QSharedPointer<services::BaseError> error);
    void handleUpdateSuccess(const models::AnimalDTO& animal);
    void handleUpdateFailed(QSharedPointer<services::BaseError> error);
    void handleFiltersLoaded(const models::AnimalFilterDTO& filters);
    void handleFiltersFailed(QSharedPointer<services::BaseError> error);
    void handleBreedsLoaded(const QList<models::BreedDTO>& breeds);
    void handleBreedsLoadFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    void setupConnections();
    void loadFilters();
    void loadBreedsForType(models::AnimalType type);
    void updateBreedsList(const QList<models::BreedDTO>& breeds);
    void updateDirtyStatus();
    void notifyAllChanged();
    void setDirty(bool dirty);

    template <typename T>
    QVariantList toVariantList(const std::optional<QVector<T>>& vec) const {
        QVariantList list;
        if (vec.has_value()) {
            for (const auto& item : *vec) {
                QVariantMap entry;
                entry["text"] = QString::number(static_cast<int>(item));
                entry["value"] = QString::number(static_cast<int>(item));
                list.append(entry);
            }
        }
        return list;
    }

    template <typename T>
    QVariantList toVariantList(const std::optional<QVector<T>>& vec, QString (*converter)(T)) const {
        QVariantList list;
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
    qint64 m_animalId = 0;

    models::AnimalDTO m_originalData;
    models::AnimalUpdateDTO m_changes;
    models::AnimalFilterDTO m_filterDto;

    std::optional<models::AnimalType> m_currentAnimalType;

    QVariantList m_breedsList;
    bool m_isLoadingBreeds = false;
    bool m_isDirty = false;
};

}  // namespace pawspective::viewmodels