#include "viewmodels/update_organization_viewmodel.hpp"
#include <QVariantMap>

namespace pawspective::viewmodels {

UpdateOrganizationViewModel::UpdateOrganizationViewModel(
    services::OrganizationService& organizationService,
    services::CityService& cityService,
    services::AuthService& authService,
    QObject* parent
)
    : BaseViewModel(parent),
      m_organizationService(organizationService),
      m_cityService(cityService),
      m_authService(authService),
      m_isDirty(false) {
    // OrganizationService connections
    connect(
        &m_organizationService,
        &services::OrganizationService::getOrganizationSuccess,
        this,
        &UpdateOrganizationViewModel::handleGetSuccess
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::updateOrganizationSuccess,
        this,
        &UpdateOrganizationViewModel::handleUpdateSuccess
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::getOrganizationFailed,
        this,
        &UpdateOrganizationViewModel::handleGetFailed
    );
    connect(
        &m_organizationService,
        &services::OrganizationService::updateOrganizationFailed,
        this,
        &UpdateOrganizationViewModel::handleUpdateFailed
    );

    // CityService connections
    connect(
        &m_cityService,
        &services::CityService::getCitiesSuccess,
        this,
        &UpdateOrganizationViewModel::handleCitiesSuccess
    );
    connect(
        &m_cityService,
        &services::CityService::getCitiesFailed,
        this,
        &UpdateOrganizationViewModel::handleCitiesFailed
    );

    // AuthService connections
    connect(
        &m_authService,
        &services::AuthService::getCurrentUserSuccess,
        this,
        &UpdateOrganizationViewModel::handleGetCurrentUserSuccess
    );
    connect(
        &m_authService,
        &services::AuthService::getCurrentUserFailed,
        this,
        &UpdateOrganizationViewModel::handleGetCurrentUserFailed
    );
}

void UpdateOrganizationViewModel::initialize() {
    if (isBusy()) {
        return;
    }
    setIsBusy(true);

    m_cityService.getCities();
    m_authService.getCurrentUser();
}

void UpdateOrganizationViewModel::cleanup() {
    setIsBusy(false);
    discardChanges();
}

QString UpdateOrganizationViewModel::name() const { return m_changes.name.value_or(m_originalData.name); }

QString UpdateOrganizationViewModel::description() const {
    return m_changes.description.value_or(m_originalData.description.value_or(""));
}

qint64 UpdateOrganizationViewModel::cityId() const { return m_changes.cityId.value_or(m_originalData.city.id); }

int UpdateOrganizationViewModel::currentCityIndex() const {
    qint64 currentId = cityId();
    for (int i = 0; i < m_cities.size(); ++i) {
        if (m_cities[i].toMap()["value"].toLongLong() == currentId) {
            return i;
        }
    }
    return 0;
}

void UpdateOrganizationViewModel::setName(const QString& value) {
    if (name() != value) {
        m_changes.name = (value == m_originalData.name) ? std::nullopt : std::make_optional(value);
        emit nameChanged();
        updateDirtyStatus();
    }
}

void UpdateOrganizationViewModel::setDescription(const QString& value) {
    if (description() != value) {
        QString original = m_originalData.description.value_or("");
        m_changes.description = (value == original) ? std::nullopt : std::make_optional(value);
        emit descriptionChanged();
        updateDirtyStatus();
    }
}

void UpdateOrganizationViewModel::setCityId(qint64 value) {
    if (cityId() != value) {
        m_changes.cityId = (value == m_originalData.city.id) ? std::nullopt : std::make_optional(value);
        emit cityIdChanged();
        emit currentCityIndexChanged();
        updateDirtyStatus();
    }
}

void UpdateOrganizationViewModel::saveChanges() {
    if (!m_isDirty || isBusy()) {
        return;
    }
    setIsBusy(true);
    m_organizationService.updateOrganization(m_originalData.id, m_changes);
}

void UpdateOrganizationViewModel::discardChanges() {
    m_changes = models::OrganizationUpdateDTO();
    setDirty(false);
    notifyAllChanged();
}

void UpdateOrganizationViewModel::handleGetCurrentUserSuccess(const models::UserDTO& user) {
    if (user.organizationId.has_value()) {
        m_organizationService.getOrganization(user.organizationId.value());
    } else {
        setIsBusy(false);
        emit loadFailed("User is not associated with any organization");
    }
}

void UpdateOrganizationViewModel::handleGetSuccess(const models::OrganizationDTO& organization) {
    setIsBusy(false);
    m_originalData = organization;
    discardChanges();
    emit loadCompleted();
}

void UpdateOrganizationViewModel::handleUpdateSuccess(const models::OrganizationDTO& organization) {
    setIsBusy(false);
    m_originalData = organization;
    m_changes = models::OrganizationUpdateDTO();
    setDirty(false);
    notifyAllChanged();
    emit saveCompleted();
}

void UpdateOrganizationViewModel::handleCitiesSuccess(const QList<models::CityDTO>& cities) {
    QVariantList list;
    for (const auto& city : cities) {
        QVariantMap entry;
        entry["text"] = city.name;
        entry["value"] = city.id;
        list.append(entry);
    }
    updateProperty(m_cities, list, [this] { emit citiesChanged(); });
    emit currentCityIndexChanged();
}

void UpdateOrganizationViewModel::handleGetFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    QString msg = error ? error->getMessage() : "Failed to load organization data";
    emit loadFailed(msg);
    emitError(ErrorType::NetworkError, msg);
}

void UpdateOrganizationViewModel::handleUpdateFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (!error) {
        emitError(ErrorType::UnknownError, "An unexpected error occurred.");
        emit saveFailed("Unknown error");
        return;
    }

    QString message;
    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        message = formatValidationError(validationError);

        emitError(ErrorType::ValidationError, message);
    } else {
        message = error->getMessage();
        emitError(ErrorType::NetworkError, message);
    }

    emit saveFailed(message);
}

void UpdateOrganizationViewModel::handleCitiesFailed(QSharedPointer<services::BaseError> error) {
    QString msg = error ? error->getMessage() : "Failed to load cities list";
    emitError(ErrorType::NetworkError, msg);
}

void UpdateOrganizationViewModel::handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    QString msg = error ? error->getMessage() : "Failed to identify current user";
    emit loadFailed(msg);
    emitError(ErrorType::NetworkError, msg);
}

void UpdateOrganizationViewModel::updateDirtyStatus() {
    bool dirty = m_changes.name.has_value() || m_changes.description.has_value() || m_changes.cityId.has_value();
    setDirty(dirty);
}

void UpdateOrganizationViewModel::setDirty(bool dirty) {
    updateProperty(m_isDirty, dirty, [this] { emit dirtyChanged(); });
}

void UpdateOrganizationViewModel::notifyAllChanged() {
    emit nameChanged();
    emit descriptionChanged();
    emit cityIdChanged();
    emit currentCityIndexChanged();
}

}  // namespace pawspective::viewmodels