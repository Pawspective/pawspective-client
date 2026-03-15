#include "viewmodels/organization_view_model.hpp"

namespace pawspective::viewmodels {

OrganizationViewModel::OrganizationViewModel(
    services::AuthService& authService,
    services::OrganizationService& organizationService,
    QObject* parent
)
    : BaseViewModel(parent), m_authService(authService), m_organizationService(organizationService) {
    connect(
        &m_authService,
        &services::AuthService::refreshFailed,
        this,
        &OrganizationViewModel::handleRefreshFailed
    );
    connect(&m_authService, &services::AuthService::loginSuccess, this, &OrganizationViewModel::handleLoginSuccess);
    connect(&m_authService, &services::AuthService::sessionEnded, this, &OrganizationViewModel::handleSessionEnded);

    connect(
        &m_organizationService,
        &services::OrganizationService::getOrganizationSuccess,
        this,
        &OrganizationViewModel::handleGetOrganizationSuccess
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::getOrganizationFailed,
        this,
        &OrganizationViewModel::handleGetOrganizationFailed
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::createOrganizationSuccess,
        this,
        &OrganizationViewModel::handleCreateOrganizationSuccess
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::createOrganizationFailed,
        this,
        &OrganizationViewModel::handleCreateOrganizationFailed
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::updateOrganizationSuccess,
        this,
        &OrganizationViewModel::handleUpdateOrganizationSuccess
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::updateOrganizationFailed,
        this,
        &OrganizationViewModel::handleUpdateOrganizationFailed
    );
}

bool OrganizationViewModel::hasOrganization() const { return m_hasOrganization; }

bool OrganizationViewModel::canUpdateOrganization() const { return m_canUpdateOrganization; }

QString OrganizationViewModel::organizationName() const { return m_organizationData.name; }

QString OrganizationViewModel::organizationCity() const { return m_organizationData.city.name; }

QString OrganizationViewModel::organizationDescription() const {
    return m_organizationData.description.has_value() ? m_organizationData.description.value() : QString();
}

bool OrganizationViewModel::showDescription() const { return m_showDescription; }

int OrganizationViewModel::currentTab() const { return m_currentTab; }

qint64 OrganizationViewModel::currentOrganizationId() const { return m_currentOrganizationId; }

void OrganizationViewModel::setShowDescription(bool value) {
    updateProperty(m_showDescription, value, [this] { emit showDescriptionChanged(); });
}

void OrganizationViewModel::setCurrentTab(int value) {
    const int normalized = qBound(0, value, 2);
    updateProperty(m_currentTab, normalized, [this] { emit currentTabChanged(); });
}

void OrganizationViewModel::initialize() {
    if (isBusy()) {
        return;
    }

    if (tryLoadCurrentOrganization()) {
        return;
    }

    initializeForCreateOrganization();
}

void OrganizationViewModel::cleanup() {
    setIsBusy(false);
    setShowDescription(false);
    setCurrentTab(2);
    clearOrganizationData();
}

void OrganizationViewModel::initializeForOrganization(qint64 organizationId) {
    if (organizationId <= 0) {
        initializeForCreateOrganization();
        return;
    }

    updateProperty(m_currentOrganizationId, organizationId, [this] { emit currentOrganizationIdChanged(); });
    loadOrganizationById(organizationId);
}

void OrganizationViewModel::initializeForCreateOrganization() {
    setIsBusy(false);
    resetOrganizationState();
}

void OrganizationViewModel::setCanUpdateOrganization(bool value) {
    updateProperty(m_canUpdateOrganization, value, [this] { emit canUpdateOrganizationChanged(); });
}

void OrganizationViewModel::refreshOrganization() {
    if (tryLoadCurrentOrganization()) {
        return;
    }

    initialize();
}

void OrganizationViewModel::createOrganization() { emit createOrganizationRequested(); }

void OrganizationViewModel::updateOrganization() {
    if (!m_hasOrganization || m_currentOrganizationId <= 0) {
        emitError(ErrorType::ValidationError, "Organization profile is not available.");
        return;
    }

    emit updateOrganizationRequested();
}

void OrganizationViewModel::handleGetCurrentUserSuccess(const models::UserDTO& user) {
    setIsBusy(false);

    if (!user.organizationId.has_value() || user.organizationId.value() <= 0) {
        initializeForCreateOrganization();
        return;
    }

    updateProperty(m_canUpdateOrganization, true, [this] { emit canUpdateOrganizationChanged(); });
    loadOrganizationById(user.organizationId.value());
}

void OrganizationViewModel::handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error) {
    handleNetworkFailure(error, true);
}

void OrganizationViewModel::handleGetOrganizationSuccess(const models::OrganizationDTO& organization) {
    applyOrganizationLoaded(organization, true);
}

void OrganizationViewModel::handleGetOrganizationFailed(QSharedPointer<services::BaseError> error) {
    handleNetworkFailure(error, true);
}

void OrganizationViewModel::handleCreateOrganizationSuccess(const models::OrganizationDTO& organization) {
    applyOrganizationLoaded(organization, true, true);
}

void OrganizationViewModel::handleCreateOrganizationFailed(QSharedPointer<services::BaseError> error) {
    handleNetworkFailure(error, false);
}

void OrganizationViewModel::handleUpdateOrganizationSuccess(const models::OrganizationDTO& organization) {
    applyOrganizationLoaded(organization, true);
}

void OrganizationViewModel::handleUpdateOrganizationFailed(QSharedPointer<services::BaseError> error) {
    handleNetworkFailure(error, false);
}

void OrganizationViewModel::handleRefreshFailed(QSharedPointer<services::BaseError> error) {
    resetOrganizationState();

    if (error) {
        emitError(ErrorType::AuthenticationError, error->getMessage());
    }
    emit sessionExpired();
}

void OrganizationViewModel::handleSessionEnded() {
    handleRefreshFailed(nullptr);
}

void OrganizationViewModel::handleLoginSuccess(const QString&, const QString&, const QString&, uint64_t) { initialize(); }

bool OrganizationViewModel::tryLoadCurrentOrganization() {
    if (m_currentOrganizationId <= 0) {
        return false;
    }

    loadOrganizationById(m_currentOrganizationId);
    return true;
}

void OrganizationViewModel::resetOrganizationState() {
    clearOrganizationData();
    updateProperty(m_currentOrganizationId, static_cast<qint64>(0), [this] { emit currentOrganizationIdChanged(); });
    updateProperty(m_hasOrganization, false, [this] { emit hasOrganizationChanged(); });
    updateProperty(m_canUpdateOrganization, false, [this] { emit canUpdateOrganizationChanged(); });
}

void OrganizationViewModel::applyOrganizationLoaded(
    const models::OrganizationDTO& organization,
    bool hasOrganizationValue,
    std::optional<bool> canUpdateOrganizationValue
) {
    setIsBusy(false);
    updateOrganizationData(organization);
    updateProperty(m_currentOrganizationId, organization.id, [this] { emit currentOrganizationIdChanged(); });
    updateProperty(m_hasOrganization, hasOrganizationValue, [this] { emit hasOrganizationChanged(); });
    if (canUpdateOrganizationValue.has_value()) {
        updateProperty(m_canUpdateOrganization, canUpdateOrganizationValue.value(), [this] { emit canUpdateOrganizationChanged(); });
    }
    emit organizationLoaded();
}

void OrganizationViewModel::handleNetworkFailure(QSharedPointer<services::BaseError> error, bool emitLoadFailedSignal) {
    setIsBusy(false);
    if (!error) {
        return;
    }

    if (emitLoadFailedSignal) {
        emit organizationLoadFailed(error->getMessage());
    }
    emitError(ErrorType::NetworkError, error->getMessage());
}

void OrganizationViewModel::loadOrganizationById(qint64 organizationId) {
    if (organizationId <= 0 || isBusy()) {
        return;
    }

    setIsBusy(true);
    m_organizationService.getOrganization(organizationId);
}

void OrganizationViewModel::updateOrganizationData(const models::OrganizationDTO& organization) {
    m_organizationData = organization;
    emit organizationDataChanged();
}

void OrganizationViewModel::clearOrganizationData() {
    m_organizationData = models::OrganizationDTO{};
    emit organizationDataChanged();
}

}  // namespace pawspective::viewmodels
