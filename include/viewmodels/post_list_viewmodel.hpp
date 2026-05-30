#pragma once

#include "models/post_dto.hpp"
#include "services/post_service.hpp"
#include "viewmodels/base.hpp"

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QSet>
#include <QSharedPointer>
#include <QVariantList>

namespace pawspective::viewmodels {

namespace detail {
class PostListInternalModel : public QAbstractListModel {
    Q_OBJECT

public:
    struct Item {
        qint64 id;
        QString text;
        QDateTime createdAt;
    };

    // NOLINTNEXTLINE(performance-enum-size)
    enum PostRole { PostIdRole = Qt::UserRole + 1, TextRole, CreatedAtRole };

    explicit PostListInternalModel(QObject* parent = nullptr);
    ~PostListInternalModel() override = default;

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void update(const QList<models::PostDTO>& dtos);
    void clear();

private:
    QList<Item> m_items;
};

}  // namespace detail

class PostListViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* listModel READ listModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalPages READ totalPages NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalCount READ totalCount NOTIFY paginationChanged)
    Q_PROPERTY(int pageSize READ pageSize NOTIFY paginationChanged)

public:
    explicit PostListViewModel(services::PostService& postService, QObject* parent = nullptr);

    ~PostListViewModel() override = default;

    QAbstractListModel* listModel();
    bool isLoading() const { return m_isLoading; }
    int currentPage() const { return m_currentPage; }
    qint64 totalPages() const { return m_totalPages; }
    qint64 totalCount() const { return m_totalCount; }
    int pageSize() const { return m_pageSize; }

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;

    Q_INVOKABLE void loadPostsForOrganization(qint64 organizationId);
    Q_INVOKABLE void goToPage(int page);
    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void prevPage();

signals:
    void isLoadingChanged();
    void paginationChanged();

private:
    void setLoading(bool loading);

    QAbstractListModel* m_listModel;
    services::PostService& m_postService;
    qint64 m_currentOrganizationId = 0;
    bool m_isLoading = false;
    int m_currentPage = 1;
    qint64 m_totalPages = 0;
    qint64 m_totalCount = 0;
    int m_pageSize = 10;

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private slots:
    void handleGetPostsSuccess(const models::PostListDTO& result);
    void handleGetPostsFailed(QSharedPointer<services::BaseError> error);
};

}  // namespace pawspective::viewmodels
