#pragma once

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVariantList>

#include "services/organization_service.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

class SearchOrganizationViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(bool isSearching READ isSearching NOTIFY isSearchingChanged)
    Q_PROPERTY(int organizationsCount READ organizationsCount NOTIFY organizationsChanged)
    Q_PROPERTY(QVariantList organizations READ organizations NOTIFY organizationsChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalPages READ totalPages NOTIFY paginationChanged)
    Q_PROPERTY(qint64 totalCount READ totalCount NOTIFY paginationChanged)

public:
    explicit SearchOrganizationViewModel(services::OrganizationService& organizationService, QObject* parent = nullptr);

    bool isSearching() const { return m_isSearching; }
    int organizationsCount() const { return m_organizationsList.size(); }
    QVariantList organizations() const { return m_organizationsList; }
    const QString& searchQuery() const { return m_searchQuery; }
    int currentPage() const { return m_currentPage; }
    qint64 totalPages() const { return m_totalPages; }
    qint64 totalCount() const { return m_totalCount; }

    void setSearchQuery(const QString& query);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void searchOrganizations();
    Q_INVOKABLE void clearResults();
    Q_INVOKABLE void goToPage(int page);
    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void prevPage();

signals:
    void isSearchingChanged();
    void organizationsChanged();
    void searchQueryChanged();
    void paginationChanged();

private slots:
    void handleSearchSuccess(const models::OrganizationListDTO& result);
    void handleSearchFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    services::OrganizationService& m_organizationService;

    bool m_isSearching = false;
    QString m_searchQuery;
    QVariantList m_organizationsList;
    int m_currentPage = 1;
    qint64 m_totalPages = 0;
    qint64 m_totalCount = 0;

    void performSearch(int page = 1);
    void updateOrganizationsList(const QList<models::OrganizationDTO>& organizations);
    void clearOrganizationsList();
};

}  // namespace pawspective::viewmodels
