#pragma once

#include "models/adopt_request_dto.hpp"
#include "services/adopt_request_service.hpp"
#include "viewmodels/base.hpp"

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QSharedPointer>

namespace pawspective::viewmodels {

namespace detail {
class AdoptRequestListInternalModel : public QAbstractListModel {
    Q_OBJECT

public:
    struct Item {
        qint64 requestId;
        QString email;
        qint64 animalId;
        QString animalName;
        QString animalBreed;
        qint32 animalAge;
        QString animalDescription;
    };

    // NOLINTNEXTLINE(performance-enum-size)
    enum RequestRole {
        RequestIdRole = Qt::UserRole + 1,
        EmailRole,
        AnimalIdRole,
        AnimalNameRole,
        AnimalBreedRole,
        AnimalAgeRole,
        AnimalDescriptionRole
    };

    explicit AdoptRequestListInternalModel(QObject* parent = nullptr);
    ~AdoptRequestListInternalModel() override = default;

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void update(const QList<models::AdoptRequestDTO>& dtos);
    void clear();

private:
    QList<Item> m_items;
};

}  // namespace detail

class AdoptRequestListViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* listModel READ listModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalPages READ totalPages NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalCount READ totalCount NOTIFY paginationChanged)
    Q_PROPERTY(int pageSize READ pageSize NOTIFY paginationChanged)

public:
    explicit AdoptRequestListViewModel(services::AdoptRequestService& adoptRequestService, QObject* parent = nullptr);
    ~AdoptRequestListViewModel() override = default;

    QAbstractListModel* listModel();
    bool isLoading() const { return m_isLoading; }
    int currentPage() const { return m_currentPage; }
    qint64 totalPages() const { return m_totalPages; }
    qint64 totalCount() const { return m_totalCount; }
    int pageSize() const { return m_pageSize; }

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;

    Q_INVOKABLE void loadRequestsForOrganization(qint64 organizationId);
    Q_INVOKABLE void goToPage(int page);
    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void prevPage();
    Q_INVOKABLE void acceptRequest(qint64 requestId);
    Q_INVOKABLE void denyRequest(qint64 requestId);

signals:
    void isLoadingChanged();
    void paginationChanged();
    void requestActionDone();

private:
    void setLoading(bool loading);
    void reloadCurrentPage();

    QAbstractListModel* m_listModel;
    services::AdoptRequestService& m_adoptRequestService;
    qint64 m_currentOrganizationId = 0;
    bool m_isLoading = false;
    int m_currentPage = 1;
    qint64 m_totalPages = 0;
    qint64 m_totalCount = 0;
    int m_pageSize = 10;

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private slots:
    void handleGetRequestsSuccess(const models::AdoptRequestListDTO& result);
    void handleGetRequestsFailed(QSharedPointer<services::BaseError> error);
    void handleAcceptSuccess();
    void handleAcceptFailed(QSharedPointer<services::BaseError> error);
    void handleDenySuccess();
    void handleDenyFailed(QSharedPointer<services::BaseError> error);
};

}  // namespace pawspective::viewmodels
