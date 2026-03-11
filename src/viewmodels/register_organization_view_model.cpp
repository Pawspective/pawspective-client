#include "viewmodels/register_organization_view_model.hpp"
#include "models/organization_register_dto.hpp"
#include "services/errors.hpp"

namespace pawspective::viewmodels {

RegisterOrganizationViewModel::RegisterOrganizationViewModel(
    services::OrganizationService& organizationService,
    QObject* parent
)
    : BaseViewModel(parent), m_organizationService(organizationService) {
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
            emitError(ValidationError, error->getMessage());
            emit registrationFinished(false);
        }
    );
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
