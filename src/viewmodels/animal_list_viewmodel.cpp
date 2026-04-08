#include "viewmodels/animal_list_viewmodel.hpp"
#include "services/errors.hpp"

namespace pawspective::viewmodels::detail {

AnimalListInternalModel::AnimalListInternalModel(QObject* parent) : QAbstractListModel(parent) {}

int AnimalListInternalModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant AnimalListInternalModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto& item = m_items.at(index.row());

    switch (role) {
        case AnimalIdRole:
            return item.id;
        case NameRole:
            return item.name;
        case DescriptionRole:
            return item.description;
        case AgeRole:
            return item.age;
        case AnimalTypeRole:
            return item.animalType;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> AnimalListInternalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[AnimalIdRole] = "animalId";
    roles[NameRole] = "animalName";
    roles[DescriptionRole] = "animalDescription";
    roles[AgeRole] = "animalAge";
    roles[AnimalTypeRole] = "animalType";
    return roles;
}

void AnimalListInternalModel::update(const QList<models::AnimalDTO>& dtos) {
    clear();

    if (!dtos.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, dtos.size() - 1);
        for (const auto& dto : dtos) {
            Item item;
            item.id = dto.id;
            item.name = dto.name;
            item.description = dto.description.value_or("");
            item.age = dto.age;
            // TODO: map animal type to human-readable string
            switch (dto.breed.animalType) {
                case models::AnimalType::Dog:
                    item.animalType = "Dog";
                    break;
                case models::AnimalType::Cat:
                    item.animalType = "Cat";
                    break;
                default:
                    item.animalType = "Other";
            }
            m_items.append(item);
        }
        endInsertRows();
    }
}

void AnimalListInternalModel::clear() {
    if (!m_items.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        m_items.clear();
        endRemoveRows();
    }
}

}  // namespace pawspective::viewmodels::detail

namespace pawspective::viewmodels {

AnimalListViewModel::AnimalListViewModel(
    services::AnimalService& animalService,
    services::OrganizationService& organizationService,
    QObject* parent
)
    : BaseViewModel(parent),
      m_listModel(new detail::AnimalListInternalModel(this)),
      m_animalService(animalService),
      m_organizationService(organizationService) {
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalsByOrganizationSuccess,
        this,
        &AnimalListViewModel::handleGetAnimalsByOrganizationSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalsByOrganizationFailed,
        this,
        &AnimalListViewModel::handleGetAnimalsByOrganizationFailed
    );
}

QAbstractListModel* AnimalListViewModel::listModel() { return m_listModel; }

void AnimalListViewModel::initialize() {
    if (m_currentOrganizationId > 0) {
        loadAnimalsForOrganization(m_currentOrganizationId);
    }
}

void AnimalListViewModel::cleanup() {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        qDebug() << "Cleaning up AnimalListViewModel, clearing internal model";
        internalModel->clear();
    }
}

void AnimalListViewModel::replaceAllAnimals(const QList<models::AnimalDTO>& animals) {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        internalModel->update(animals);
    }
}

void AnimalListViewModel::loadAnimalsForOrganization(qint64 organizationId) {
    m_currentOrganizationId = organizationId;
    if (organizationId <= 0) {
        if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
            internalModel->clear();
        }
        return;
    }

    setIsBusy(true);
    m_animalService.getAnimalsByOrganization(organizationId);
}

void AnimalListViewModel::handleGetAnimalsByOrganizationSuccess(const QList<models::AnimalDTO>& animals) {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        qDebug() << "Received" << animals.size() << "animals for organization" << m_currentOrganizationId;
        internalModel->update(animals);
    }
    setIsBusy(false);
}

void AnimalListViewModel::handleGetAnimalsByOrganizationFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (error) {
        qWarning()
            << "Failed to load animals for organization" << m_currentOrganizationId << ":" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

}  // namespace pawspective::viewmodels
