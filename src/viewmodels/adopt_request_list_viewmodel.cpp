#include "viewmodels/adopt_request_list_viewmodel.hpp"

#include <QDebug>

#include "services/errors.hpp"

namespace pawspective::viewmodels::detail {

AdoptRequestListInternalModel::AdoptRequestListInternalModel(QObject* parent) : QAbstractListModel(parent) {}

int AdoptRequestListInternalModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant AdoptRequestListInternalModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto& item = m_items.at(index.row());

    switch (role) {
        case RequestIdRole:
            return item.requestId;
        case EmailRole:
            return item.email;
        case AnimalIdRole:
            return item.animalId;
        case AnimalNameRole:
            return item.animalName;
        case AnimalBreedRole:
            return item.animalBreed;
        case AnimalAgeRole:
            return item.animalAge;
        case AnimalDescriptionRole:
            return item.animalDescription;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> AdoptRequestListInternalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[RequestIdRole] = "requestId";
    roles[EmailRole] = "requestEmail";
    roles[AnimalIdRole] = "animalId";
    roles[AnimalNameRole] = "animalName";
    roles[AnimalBreedRole] = "animalBreed";
    roles[AnimalAgeRole] = "animalAge";
    roles[AnimalDescriptionRole] = "animalDescription";
    return roles;
}

void AdoptRequestListInternalModel::update(const QList<models::AdoptRequestDTO>& dtos) {
    clear();

    if (!dtos.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, dtos.size() - 1);
        for (const auto& dto : dtos) {
            Item item;
            item.requestId = dto.id;
            item.email = dto.email;
            item.animalId = dto.animal.id;
            item.animalName = dto.animal.name;
            item.animalBreed = dto.animal.breed.name;
            item.animalAge = dto.animal.age;
            item.animalDescription = dto.animal.description.value_or(QString{});
            m_items.append(item);
        }
        endInsertRows();
    }
}

void AdoptRequestListInternalModel::clear() {
    if (!m_items.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        m_items.clear();
        endRemoveRows();
    }
}

}  // namespace pawspective::viewmodels::detail

namespace pawspective::viewmodels {

AdoptRequestListViewModel::AdoptRequestListViewModel(
    services::AdoptRequestService& adoptRequestService,
    QObject* parent
)
    : BaseViewModel(parent)
    , m_listModel(new detail::AdoptRequestListInternalModel(this))
    , m_adoptRequestService(adoptRequestService) {
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::getAdoptRequestsSuccess,
        this,
        &AdoptRequestListViewModel::handleGetRequestsSuccess
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::getAdoptRequestsFailed,
        this,
        &AdoptRequestListViewModel::handleGetRequestsFailed
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::acceptAdoptRequestSuccess,
        this,
        &AdoptRequestListViewModel::handleAcceptSuccess
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::acceptAdoptRequestFailed,
        this,
        &AdoptRequestListViewModel::handleAcceptFailed
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::denyAdoptRequestSuccess,
        this,
        &AdoptRequestListViewModel::handleDenySuccess
    );
    connect(
        &m_adoptRequestService,
        &services::AdoptRequestService::denyAdoptRequestFailed,
        this,
        &AdoptRequestListViewModel::handleDenyFailed
    );
}

QAbstractListModel* AdoptRequestListViewModel::listModel() { return m_listModel; }

void AdoptRequestListViewModel::initialize() {}

void AdoptRequestListViewModel::cleanup() {
    if (auto internalModel = qobject_cast<detail::AdoptRequestListInternalModel*>(m_listModel)) {
        internalModel->clear();
    }
}

void AdoptRequestListViewModel::loadRequestsForOrganization(qint64 organizationId) {
    m_currentOrganizationId = organizationId;
    if (organizationId <= 0) {
        if (auto internalModel = qobject_cast<detail::AdoptRequestListInternalModel*>(m_listModel)) {
            internalModel->clear();
        }
        return;
    }

    m_currentPage = 1;
    m_totalPages = 0;
    m_totalCount = 0;
    emit paginationChanged();

    setLoading(true);
    m_adoptRequestService.getAdoptRequests(organizationId, m_currentPage);
}

void AdoptRequestListViewModel::goToPage(int page) {
    if (page < 1 || (m_totalPages > 0 && page > m_totalPages)) {
        return;
    }

    m_currentPage = page;
    setLoading(true);
    m_adoptRequestService.getAdoptRequests(m_currentOrganizationId, m_currentPage);
}

void AdoptRequestListViewModel::nextPage() {
    if (m_currentPage < m_totalPages) {
        goToPage(m_currentPage + 1);
    }
}

void AdoptRequestListViewModel::prevPage() {
    if (m_currentPage > 1) {
        goToPage(m_currentPage - 1);
    }
}

void AdoptRequestListViewModel::acceptRequest(qint64 requestId) {
    setLoading(true);
    m_adoptRequestService.acceptAdoptRequest(requestId);
}

void AdoptRequestListViewModel::denyRequest(qint64 requestId) {
    setLoading(true);
    m_adoptRequestService.denyAdoptRequest(requestId);
}

void AdoptRequestListViewModel::reloadCurrentPage() {
    if (m_currentOrganizationId > 0) {
        m_adoptRequestService.getAdoptRequests(m_currentOrganizationId, m_currentPage);
    }
}

void AdoptRequestListViewModel::handleGetRequestsSuccess(const models::AdoptRequestListDTO& result) {
    if (auto internalModel = qobject_cast<detail::AdoptRequestListInternalModel*>(m_listModel)) {
        qDebug() << "Received" << result.items.size() << "adopt requests (page" << result.page << "of"
                 << result.totalPages << ")";
        internalModel->update(result.items);
    }
    m_currentPage = result.page;
    m_totalPages = result.totalPages;
    m_totalCount = result.totalCount;
    m_pageSize = result.limit > 0 ? result.limit : m_pageSize;
    emit paginationChanged();
    setLoading(false);
}

void AdoptRequestListViewModel::handleGetRequestsFailed(QSharedPointer<services::BaseError> error) {
    setLoading(false);
    if (error) {
        qWarning() << "Failed to load adopt requests:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AdoptRequestListViewModel::handleAcceptSuccess() {
    emit requestActionDone();
    reloadCurrentPage();
}

void AdoptRequestListViewModel::handleAcceptFailed(QSharedPointer<services::BaseError> error) {
    setLoading(false);
    if (error) {
        qWarning() << "Failed to accept adopt request:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AdoptRequestListViewModel::handleDenySuccess() {
    emit requestActionDone();
    reloadCurrentPage();
}

void AdoptRequestListViewModel::handleDenyFailed(QSharedPointer<services::BaseError> error) {
    setLoading(false);
    if (error) {
        qWarning() << "Failed to deny adopt request:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AdoptRequestListViewModel::setLoading(bool loading) {
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

}  // namespace pawspective::viewmodels
