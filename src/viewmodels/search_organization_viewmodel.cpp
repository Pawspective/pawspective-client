#include "../../include/viewmodels/search_organization_viewmodel.hpp"
#include "viewmodels/organization_card_viewmodel.hpp"

namespace pawspective::viewmodels {

SearchOrganizationViewModel::SearchOrganizationViewModel(
    services::OrganizationService& organizationService,
    QObject* parent
)
    : BaseViewModel(parent), m_organizationService(organizationService) {
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
        performSearch(1);
    }
}

void SearchOrganizationViewModel::clearResults() {
    clearOrganizationsList();
    m_currentPage = 1;
    m_totalPages = 0;
    m_totalCount = 0;
    emit paginationChanged();
    updateProperty(m_isSearching, false, [this]() { emit isSearchingChanged(); });
}

void SearchOrganizationViewModel::goToPage(int page) {
    if (page < 1 || page > m_totalPages || isBusy()) {
        return;
    }
    performSearch(page);
}

void SearchOrganizationViewModel::nextPage() {
    if (m_currentPage < m_totalPages) {
        goToPage(m_currentPage + 1);
    }
}

void SearchOrganizationViewModel::prevPage() {
    if (m_currentPage > 1) {
        goToPage(m_currentPage - 1);
    }
}

void SearchOrganizationViewModel::performSearch(int page) {
    if (isBusy()) {
        return;
    }

    setIsBusy(true);
    updateProperty(m_isSearching, true, [this]() { emit isSearchingChanged(); });
    m_organizationService.findByNameContaining(m_searchQuery, page, m_pageSize);
}

void SearchOrganizationViewModel::handleSearchSuccess(const models::OrganizationListDTO& result) {
    setIsBusy(false);
    updateProperty(m_isSearching, false, [this]() { emit isSearchingChanged(); });

    m_currentPage = result.page;
    m_totalPages = result.totalPages;
    m_totalCount = result.totalCount;
    m_pageSize = result.limit > 0 ? result.limit : m_pageSize;
    emit paginationChanged();

    if (result.items.isEmpty()) {
        clearOrganizationsList();
    } else {
        updateOrganizationsList(result.items);
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

}  // namespace pawspective::viewmodels