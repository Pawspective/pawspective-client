#include "viewmodels/post_list_viewmodel.hpp"

#include <QDebug>

#include "services/errors.hpp"

namespace pawspective::viewmodels::detail {

PostListInternalModel::PostListInternalModel(QObject* parent) : QAbstractListModel(parent) {}

int PostListInternalModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant PostListInternalModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto& item = m_items.at(index.row());

    switch (role) {
        case PostIdRole:
            return item.id;
        case TextRole:
            return item.text;
        case CreatedAtRole:
            return item.createdAt;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> PostListInternalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[PostIdRole] = "postId";
    roles[TextRole] = "postText";
    roles[CreatedAtRole] = "postCreatedAt";
    return roles;
}

void PostListInternalModel::update(const QList<models::PostDTO>& dtos) {
    clear();

    if (!dtos.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, dtos.size() - 1);
        for (const auto& dto : dtos) {
            Item item;
            item.id = dto.id;
            item.text = dto.text;
            item.createdAt = dto.createdAt;
            m_items.append(item);
        }
        endInsertRows();
    }
}

void PostListInternalModel::clear() {
    if (!m_items.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        m_items.clear();
        endRemoveRows();
    }
}

}  // namespace pawspective::viewmodels::detail

namespace pawspective::viewmodels {

PostListViewModel::PostListViewModel(services::PostService& postService, QObject* parent)
    : BaseViewModel(parent), m_listModel(new detail::PostListInternalModel(this)), m_postService(postService) {
    connect(
        &m_postService,
        &services::PostService::getByOrganizationIdSuccess,
        this,
        &PostListViewModel::handleGetPostsSuccess
    );
    connect(
        &m_postService,
        &services::PostService::getByOrganizationIdFailed,
        this,
        &PostListViewModel::handleGetPostsFailed
    );
    connect(
        &m_postService,
        &services::PostService::deletePostSuccess,
        this,
        &PostListViewModel::handleDeletePostSuccess
    );
    connect(
        &m_postService,
        &services::PostService::deletePostFailed,
        this,
        &PostListViewModel::handleDeletePostFailed
    );
}

QAbstractListModel* PostListViewModel::listModel() { return m_listModel; }

void PostListViewModel::initialize() {}

void PostListViewModel::cleanup() {
    if (auto internalModel = qobject_cast<detail::PostListInternalModel*>(m_listModel)) {
        qDebug() << "Cleaning up PostListViewModel, clearing internal model";
        internalModel->clear();
    }
}

void PostListViewModel::loadPostsForOrganization(qint64 organizationId) {
    m_currentOrganizationId = organizationId;
    if (organizationId <= 0) {
        if (auto internalModel = qobject_cast<detail::PostListInternalModel*>(m_listModel)) {
            internalModel->clear();
        }
        return;
    }

    m_currentOrganizationId = organizationId;
    m_currentPage = 1;
    m_totalPages = 0;
    m_totalCount = 0;
    emit paginationChanged();

    setLoading(true);
    m_postService.getByOrganizationId(organizationId);
}

void PostListViewModel::goToPage(int page) {
    if (page < 1 || (m_totalPages > 0 && page > m_totalPages)) {
        return;
    }

    m_currentPage = page;
    setLoading(true);
    m_postService.getByOrganizationId(m_currentOrganizationId, m_currentPage, m_pageSize);
}

void PostListViewModel::nextPage() {
    if (m_currentPage < m_totalPages) {
        goToPage(m_currentPage + 1);
    }
}

void PostListViewModel::prevPage() {
    if (m_currentPage > 1) {
        goToPage(m_currentPage - 1);
    }
}

void PostListViewModel::handleGetPostsSuccess(const models::PostListDTO& result) {
    if (auto internalModel = qobject_cast<detail::PostListInternalModel*>(m_listModel)) {
        qDebug()
            << "Received" << result.items.size() << "posts by filters (page" << result.page << "of" << result.totalPages
            << ")";
        internalModel->update(result.items);
    }
    m_currentPage = result.page;
    m_totalPages = result.totalPages;
    m_totalCount = result.totalCount;
    m_pageSize = result.limit > 0 ? result.limit : m_pageSize;
    emit paginationChanged();
    setLoading(false);
}

void PostListViewModel::handleGetPostsFailed(QSharedPointer<services::BaseError> error) {
    setLoading(false);
    if (error) {
        qWarning() << "Failed to load posts by filters:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void PostListViewModel::setLoading(bool loading) {
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

void PostListViewModel::deletePost(qint64 postId) {
    if (postId <= 0) {
        emit deleteFailed("Invalid post ID");
        return;
    }
    setLoading(true);
    m_postService.deletePost(postId);
}

void PostListViewModel::handleDeletePostSuccess() {
    setLoading(false);
    emit deleteSuccess();
    if (m_currentOrganizationId > 0) {
        loadPostsForOrganization(m_currentOrganizationId);
    }
}

void PostListViewModel::handleDeletePostFailed(QSharedPointer<services::BaseError> error) {
    setLoading(false);
    QString message = error ? error->getMessage() : "Failed to delete post";
    emit deleteFailed(message);
    emitError(ErrorType::NetworkError, message);
}

}  // namespace pawspective::viewmodels
