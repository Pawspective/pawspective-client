#include "viewmodels/review_list_viewmodel.hpp"

#include <QDebug>

#include "services/errors.hpp"

namespace pawspective::viewmodels::detail {

ReviewListInternalModel::ReviewListInternalModel(QObject* parent) : QAbstractListModel(parent) {}

int ReviewListInternalModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant ReviewListInternalModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto& item = m_items.at(index.row());

    switch (role) {
        case ReviewIdRole:
            return item.id;
        case AnimalIdRole:
            return item.animalId;
        case AnimalNameRole:
            return item.animalName;
        case TextRole:
            return item.text;
        case CreatedAtRole:
            return item.createdAt;
        case CanEditRole:
            return item.canEdit;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ReviewListInternalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ReviewIdRole] = "id";
    roles[AnimalIdRole] = "animalId";
    roles[AnimalNameRole] = "animalName";
    roles[TextRole] = "text";
    roles[CreatedAtRole] = "createdAt";
    roles[CanEditRole] = "canEdit";
    return roles;
}

void ReviewListInternalModel::update(const QList<models::ReviewDTO>& dtos) {
    clear();

    if (!dtos.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, dtos.size() - 1);
        for (const auto& dto : dtos) {
            m_items.append(dto);
        }
        endInsertRows();
    }
}

void ReviewListInternalModel::clear() {
    if (!m_items.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        m_items.clear();
        endRemoveRows();
    }
}

}  // namespace pawspective::viewmodels::detail

namespace pawspective::viewmodels {

ReviewListViewModel::ReviewListViewModel(services::ReviewService& reviewService, QObject* parent)
    : BaseViewModel(parent), m_listModel(new detail::ReviewListInternalModel(this)), m_reviewService(reviewService) {
    connect(
        &m_reviewService,
        &services::ReviewService::getByOrganizationIdSuccess,
        this,
        &ReviewListViewModel::handleGetReviewsSuccess
    );
    connect(
        &m_reviewService,
        &services::ReviewService::getByOrganizationIdFailed,
        this,
        &ReviewListViewModel::handleGetReviewsFailed
    );
    connect(
        &m_reviewService,
        &services::ReviewService::deleteReviewSuccess,
        this,
        &ReviewListViewModel::handleDeleteSuccess
    );
    connect(
        &m_reviewService,
        &services::ReviewService::deleteReviewFailed,
        this,
        &ReviewListViewModel::handleDeleteFailed
    );
}

QAbstractListModel* ReviewListViewModel::listModel() { return m_listModel; }

void ReviewListViewModel::initialize() {
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
}

void ReviewListViewModel::cleanup() {
    if (auto internalModel = qobject_cast<detail::ReviewListInternalModel*>(m_listModel)) {
        qDebug() << "Cleaning up ReviewListViewModel, clearing internal model";
        internalModel->clear();
    }
}

void ReviewListViewModel::replaceAllReviews(const QList<models::ReviewDTO>& reviews) {
    if (auto internalModel = qobject_cast<detail::ReviewListInternalModel*>(m_listModel)) {
        internalModel->update(reviews);
    }
}

void ReviewListViewModel::loadReviewsForOrganization(qint64 organizationId) {
    m_currentOrganizationId = organizationId;
    if (organizationId <= 0) {
        if (auto internalModel = qobject_cast<detail::ReviewListInternalModel*>(m_listModel)) {
            internalModel->clear();
        }
        return;
    }

    m_currentPage = 1;
    m_totalPages = 0;
    m_totalCount = 0;
    emit paginationChanged();

    updateProperty(m_isLoading, true, [this]() { emit isLoadingChanged(); });
    setIsBusy(true);
    m_reviewService.getByOrganizationId(organizationId, m_currentPage, m_pageSize);
}

void ReviewListViewModel::goToPage(int page) {
    if (page < 1 || (m_totalPages > 0 && page > m_totalPages)) {
        return;
    }

    if (m_currentOrganizationId <= 0) {
        return;
    }

    m_currentPage = page;
    updateProperty(m_isLoading, true, [this]() { emit isLoadingChanged(); });
    setIsBusy(true);
    m_reviewService.getByOrganizationId(m_currentOrganizationId, m_currentPage, m_pageSize);
}

void ReviewListViewModel::nextPage() {
    if (m_currentPage < m_totalPages) {
        goToPage(m_currentPage + 1);
    }
}

void ReviewListViewModel::prevPage() {
    if (m_currentPage > 1) {
        goToPage(m_currentPage - 1);
    }
}

void ReviewListViewModel::deleteReview(qint64 id) {
    qDebug() << "Deleting review with ID:" << id;
    if (id <= 0) {
        emit deleteFailed("No review selected");
        return;
    }
    setIsBusy(true);
    m_reviewService.deleteReview(id);
}

void ReviewListViewModel::handleGetReviewsSuccess(const models::ReviewListDTO& result) {
    if (auto internalModel = qobject_cast<detail::ReviewListInternalModel*>(m_listModel)) {
        qDebug()
            << "Received" << result.items.size() << "reviews for organization" << m_currentOrganizationId << "(page"
            << result.page << "of" << result.totalPages << ")";
        internalModel->update(result.items);
    }

    m_currentPage = result.page;
    m_totalPages = result.totalPages;
    m_totalCount = result.totalCount;
    m_pageSize = result.limit > 0 ? result.limit : m_pageSize;
    emit paginationChanged();
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
    setIsBusy(false);
}

void ReviewListViewModel::handleGetReviewsFailed(QSharedPointer<services::BaseError> error) {
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
    setIsBusy(false);
    if (error) {
        qWarning()
            << "Failed to load reviews for organization" << m_currentOrganizationId << ":" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void ReviewListViewModel::handleDeleteSuccess() {
    updateProperty(m_isLoading, true, [this]() { emit isLoadingChanged(); });
    setIsBusy(true);
    m_reviewService.getByOrganizationId(m_currentOrganizationId, m_currentPage, m_pageSize);
    emit deleteSuccess();
}

void ReviewListViewModel::handleDeleteFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    QString message = error ? error->getMessage() : "Failed to delete review";
    emit deleteFailed(message);
    emitError(ErrorType::NetworkError, message);
}

}  // namespace pawspective::viewmodels
