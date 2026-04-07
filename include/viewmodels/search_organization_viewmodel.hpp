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

public:
    explicit SearchOrganizationViewModel(services::OrganizationService& organizationService, QObject* parent = nullptr);

    bool isSearching() const { return m_isSearching; }
    int organizationsCount() const { return m_organizationsList.size(); }
    QVariantList organizations() const { return m_organizationsList; }
    const QString& searchQuery() const { return m_searchQuery; }

    void setSearchQuery(const QString& query);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void searchOrganizations();
    Q_INVOKABLE void clearResults();

signals:
    void isSearchingChanged();
    void organizationsChanged();
    void searchQueryChanged();

private slots:
    void handleSearchSuccess(const QList<models::OrganizationDTO>& organizations);
    void handleSearchFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    services::OrganizationService& m_organizationService;

    bool m_isSearching = false;
    QString m_searchQuery;
    QVariantList m_organizationsList;

    void performSearch();
    void updateOrganizationsList(const QList<models::OrganizationDTO>& organizations);
    void clearOrganizationsList();
};

}  // namespace pawspective::viewmodels