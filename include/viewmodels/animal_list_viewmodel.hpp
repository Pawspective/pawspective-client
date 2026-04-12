#pragma once

#include "models/animal_dto.hpp"
#include "services/animal_service.hpp"
#include "services/breed_service.hpp"
#include "services/organization_service.hpp"
#include "viewmodels/base.hpp"

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QSet>
#include <QSharedPointer>
#include <QVariantList>

namespace pawspective::viewmodels {

namespace detail {

/**
 * @brief Internal list model for AnimalListViewModel
 *
 * This is an implementation detail of AnimalListViewModel and should not be used directly.
 */
class AnimalListInternalModel : public QAbstractListModel {
    Q_OBJECT

public:
    struct Item {
        qint64 id;
        QString name;
        QString description;
        qint32 age;
        QString animalType;
    };

    // NOLINTNEXTLINE(performance-enum-size)
    enum AnimalRole {
        AnimalIdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        AgeRole,
        AnimalTypeRole,
        ViewModelRole
    };

    explicit AnimalListInternalModel(QObject* parent = nullptr);
    ~AnimalListInternalModel() override = default;

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void update(const QList<models::AnimalDTO>& dtos);
    void clear();

private:
    QList<Item> m_items;
};

}  // namespace detail

class AnimalListViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* listModel READ listModel CONSTANT)
    Q_PROPERTY(QVariantList availableBreeds READ availableBreeds NOTIFY availableFiltersChanged)
    Q_PROPERTY(QVariantList availableAnimalTypes READ availableAnimalTypes NOTIFY availableFiltersChanged)
    Q_PROPERTY(QVariantList availableSizes READ availableSizes NOTIFY availableFiltersChanged)
    Q_PROPERTY(QVariantList availableGenders READ availableGenders NOTIFY availableFiltersChanged)
    Q_PROPERTY(QVariantList availableCareLevels READ availableCareLevels NOTIFY availableFiltersChanged)
    Q_PROPERTY(QVariantList availableColors READ availableColors NOTIFY availableFiltersChanged)
    Q_PROPERTY(QVariantList availableGoodWiths READ availableGoodWiths NOTIFY availableFiltersChanged)

public:
    explicit AnimalListViewModel(
        services::AnimalService& animalService,
        services::BreedService& breedService,
        services::OrganizationService& organizationService,
        QObject* parent = nullptr
    );

    ~AnimalListViewModel() override = default;

    QAbstractListModel* listModel();
    QVariantList availableBreeds() const { return m_availableBreeds; }
    QVariantList availableAnimalTypes() const { return m_availableAnimalTypes; }
    QVariantList availableSizes() const { return m_availableSizes; }
    QVariantList availableGenders() const { return m_availableGenders; }
    QVariantList availableCareLevels() const { return m_availableCareLevels; }
    QVariantList availableColors() const { return m_availableColors; }
    QVariantList availableGoodWiths() const { return m_availableGoodWiths; }

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;

    Q_INVOKABLE void replaceAllAnimals(const QList<models::AnimalDTO>& animals);
    Q_INVOKABLE void loadAnimalsForOrganization(qint64 organizationId);
    Q_INVOKABLE void loadAnimalByFilters(const QVariantMap& filterData);
    Q_INVOKABLE void loadAvailableFilters();
    Q_INVOKABLE void loadBreedsForAnimalTypes(const QVariantList& selectedTypes);

signals:
    void availableFiltersChanged();

private:
    QAbstractListModel* m_listModel;
    services::AnimalService& m_animalService;
    services::BreedService& m_breedService;
    services::OrganizationService& m_organizationService;
    QVariantList m_availableBreeds;
    QVariantList m_availableAnimalTypes;
    QVariantList m_availableSizes;
    QVariantList m_availableGenders;
    QVariantList m_availableCareLevels;
    QVariantList m_availableColors;
    QVariantList m_availableGoodWiths;
    QSet<models::AnimalType> m_requestedBreedTypes;
    qint64 m_currentOrganizationId = 0;

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private slots:
    void handleGetAnimalsSuccess(const QList<models::AnimalDTO>& animals);
    void handleGetAnimalsFailed(QSharedPointer<services::BaseError> error);
    void handleGetAnimalsByOrganizationSuccess(const QList<models::AnimalDTO>& animals);
    void handleGetAnimalsByOrganizationFailed(QSharedPointer<services::BaseError> error);
    void handleGetAnimalFiltersSuccess(const models::AnimalFilterDTO& filters);
    void handleGetAnimalFiltersFailed(QSharedPointer<services::BaseError> error);
    void handleGetBreedsSuccess(const QList<models::BreedDTO>& breeds);
    void handleGetBreedsFailed(QSharedPointer<services::BaseError> error);
};

}  // namespace pawspective::viewmodels
