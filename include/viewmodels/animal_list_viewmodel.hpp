#pragma once

#include "models/animal_dto.hpp"
#include "services/animal_service.hpp"
#include "services/organization_service.hpp"
#include "viewmodels/base.hpp"

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QSharedPointer>

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

public:
    explicit AnimalListViewModel(
        services::AnimalService& animalService,
        services::OrganizationService& organizationService,
        QObject* parent = nullptr
    );

    ~AnimalListViewModel() override = default;

    QAbstractListModel* listModel();

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;

    Q_INVOKABLE void replaceAllAnimals(const QList<models::AnimalDTO>& animals);
    Q_INVOKABLE void loadAnimalsForOrganization(qint64 organizationId);

private:
    QAbstractListModel* m_listModel;
    services::AnimalService& m_animalService;
    services::OrganizationService& m_organizationService;
    // TODO: add filter state
    qint64 m_currentOrganizationId = 0;

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private slots:
    void handleGetAnimalsByOrganizationSuccess(const QList<models::AnimalDTO>& animals);
    void handleGetAnimalsByOrganizationFailed(QSharedPointer<services::BaseError> error);
};

}  // namespace pawspective::viewmodels
