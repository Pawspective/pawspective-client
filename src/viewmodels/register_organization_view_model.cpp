#include "viewmodels/register_organization_view_model.hpp"

#include <QVariantMap>

#include "models/organization_register_dto.hpp"
#include "services/errors.hpp"

namespace pawspective::viewmodels {

RegisterOrganizationViewModel::RegisterOrganizationViewModel(
    services::OrganizationService& organizationService,
    services::CityService& cityService,
    QObject* parent
)
    : BaseViewModel(parent), m_organizationService(organizationService), m_cityService(cityService) {
    connect(
        &m_organizationService,
        &services::OrganizationService::createOrganizationSuccess,
        this,
        [this](const models::OrganizationDTO&) {
            setIsBusy(false);
            emit registrationFinished(true);
        }
    );

    connect(
        &m_organizationService,
        &services::OrganizationService::createOrganizationFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            setIsBusy(false);
            if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
                emitError(
                    ValidationError,
                    validationError->getErrors().empty()
                        ? validationError->getMessage()
                        : QString::fromStdString(validationError->getErrors()[0].errorMessage)
                );
            } else {
                emitError(NetworkError, error->getMessage());
            }
            emit registrationFinished(false);
        }
    );

    connect(
        &m_cityService,
        &services::CityService::getCitiesSuccess,
        this,
        [this](const QList<models::CityDTO>& cities) {
            QVariantList list;
            for (const models::CityDTO& city : cities) {
                QVariantMap entry;
                entry["text"] = city.name;
                entry["value"] = city.id;
                list.append(entry);
            }
            updateProperty(m_cities, list, [this] { emit citiesChanged(); });
            setIsBusy(false);
        }
    );

    connect(
        &m_cityService,
        &services::CityService::getCitiesFailed,
        this,
        [this](QSharedPointer<services::BaseError> error) {
            setIsBusy(false);
            emitError(NetworkError, error->getMessage());
        }
    );
}

void RegisterOrganizationViewModel::initialize() { loadCities(); }

void RegisterOrganizationViewModel::loadCities() {
    setIsBusy(true);
    m_cityService.getCities();
}

void RegisterOrganizationViewModel::registerOrganization() {
    if (m_name.isEmpty()) {
        emitError(ErrorType::ValidationError, "Organization name is required.");
        return;
    }

    if (m_cityId <= 0) {
        emitError(ErrorType::ValidationError, "City is required.");
        return;
    }

    pawspective::models::OrganizationRegisterDTO dto{
        m_name,
        m_description.isEmpty() ? std::optional<QString>{} : std::optional<QString>{m_description},
        m_cityId
    };
    setIsBusy(true);

    m_organizationService.createOrganization(dto);
}

}  // namespace pawspective::viewmodels
