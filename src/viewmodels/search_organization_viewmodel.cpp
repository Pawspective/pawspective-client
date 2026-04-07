#include "../../include/viewmodels/search_organization_viewmodel.hpp"
#include "viewmodels/organization_card_viewmodel.hpp"

namespace pawspective::viewmodels {

SearchOrganizationViewModel::SearchOrganizationViewModel(
    services::OrganizationService& organizationService,
    QObject* parent
) : BaseViewModel(parent), m_organizationService(organizationService) 
{
    connect(
        &m_organizationService,
        &services::OrganizationService::findByNameContainingSuccess,
        this,
        &SearchOrganizationViewModel::handleSearchSuccess
    );
    
    connect(
        &m_organizationService,
        &services::OrganizationService::findByNameContainingFailed,
        this,
        &SearchOrganizationViewModel::handleSearchFailed
    );
}

void SearchOrganizationViewModel::initialize() {
    clearResults();
    setIsBusy(false);
    m_searchQuery.clear();
    emit searchQueryChanged();
}

void SearchOrganizationViewModel::cleanup() {
    setIsBusy(false);
    clearResults();
    m_searchQuery.clear();
    emit searchQueryChanged();
}

void SearchOrganizationViewModel::setSearchQuery(const QString& query) {
    if (m_searchQuery == query) {
        return;
    }
    
    m_searchQuery = query;
    emit searchQueryChanged();
}

void SearchOrganizationViewModel::searchOrganizations() {
    if (m_searchQuery.length() >= 1 && !isBusy()) {
        performSearch();
    }
}

void SearchOrganizationViewModel::clearResults() {
    clearOrganizationsList();
    updateProperty(m_isSearching, false, [this]() { emit isSearchingChanged(); });
}

void SearchOrganizationViewModel::performSearch() {
    if (isBusy()) {
        return;
    }
    
    setIsBusy(true);
    updateProperty(m_isSearching, true, [this]() { emit isSearchingChanged(); });
    m_organizationService.findByNameContaining(m_searchQuery);
}

void SearchOrganizationViewModel::handleSearchSuccess(const QList<models::OrganizationDTO>& organizations) {
    setIsBusy(false);
    updateProperty(m_isSearching, false, [this]() { emit isSearchingChanged(); });
    
    if (organizations.isEmpty()) {
        clearOrganizationsList();
    } else {
        updateOrganizationsList(organizations);
    }
}

void SearchOrganizationViewModel::handleSearchFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    updateProperty(m_isSearching, false, [this]() { emit isSearchingChanged(); });
    
    if (error) {
        emitError(ErrorType::NetworkError, error->getMessage());
    }
    
    clearOrganizationsList();
}

void SearchOrganizationViewModel::updateOrganizationsList(const QList<models::OrganizationDTO>& organizations) {
    clearOrganizationsList();

    for (const auto& org : organizations) {
        auto* cardVM = new OrganizationCardViewModel(org, this);
        m_organizationsList.append(QVariant::fromValue(cardVM));
    }
    
    emit organizationsChanged();
}

void SearchOrganizationViewModel::clearOrganizationsList() {
    for (const auto& item : m_organizationsList) {
        auto* obj = item.value<QObject*>();
        if (obj) {
            obj->deleteLater();
        }
    }
    
    m_organizationsList.clear();
    emit organizationsChanged();
}

} // namespace pawspective::viewmodels