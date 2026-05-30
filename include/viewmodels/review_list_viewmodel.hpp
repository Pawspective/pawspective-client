#pragma once

#include "models/review_dto.hpp"
#include "services/review_service.hpp"
#include "viewmodels/base.hpp"

#include <QAbstractListModel>
#include <QHash>
#include <QList>

namespace pawspective::viewmodels {

namespace detail {

class ReviewListInternalModel : public QAbstractListModel {
    Q_OBJECT

public:
    using Item = models::ReviewDTO;

    // NOLINTNEXTLINE(performance-enum-size)
    enum ReviewRole {
        ReviewIdRole = Qt::UserRole + 1,
        AnimalIdRole,
        AnimalNameRole,
        TextRole,
        CreatedAtRole,
        CanEditRole
    };

    explicit ReviewListInternalModel(QObject* parent = nullptr);
    ~ReviewListInternalModel() override = default;

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void update(const QList<models::ReviewDTO>& dtos);
    void clear();

private:
    QList<Item> m_items;
};

}  // namespace detail

class ReviewListViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* listModel READ listModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalPages READ totalPages NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalCount READ totalCount NOTIFY paginationChanged)
    Q_PROPERTY(int pageSize READ pageSize NOTIFY paginationChanged)

public:
    explicit ReviewListViewModel(services::ReviewService& reviewService, QObject* parent = nullptr);

    ~ReviewListViewModel() override = default;

    QAbstractListModel* listModel();
    bool isLoading() const { return m_isLoading; }
    int currentPage() const { return m_currentPage; }
    qint64 totalPages() const { return m_totalPages; }
    qint64 totalCount() const { return m_totalCount; }
    int pageSize() const { return m_pageSize; }

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void replaceAllReviews(const QList<models::ReviewDTO>& reviews);
    Q_INVOKABLE void loadReviewsForOrganization(qint64 organizationId);
    Q_INVOKABLE void goToPage(int page);
    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void prevPage();

signals:
    void isLoadingChanged();
    void paginationChanged();

private:
    QAbstractListModel* m_listModel;
    services::ReviewService& m_reviewService;
    qint64 m_currentOrganizationId = 0;
    bool m_isLoading = false;
    int m_currentPage = 1;
    qint64 m_totalPages = 0;
    qint64 m_totalCount = 0;
    int m_pageSize = 10;

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private slots:
    void handleGetReviewsSuccess(const models::ReviewListDTO& result);
    void handleGetReviewsFailed(QSharedPointer<services::BaseError> error);
};

}  // namespace pawspective::viewmodels
