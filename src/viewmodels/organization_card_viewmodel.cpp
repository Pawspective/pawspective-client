#include "viewmodels/organization_card_viewmodel.hpp"

namespace pawspective::viewmodels {

    static const QString StorageBaseUrl = "https://storage.yandexcloud.net/hollow1crown/photos/";

OrganizationCardViewModel::OrganizationCardViewModel(QObject* parent) : QObject(parent) {}

OrganizationCardViewModel::OrganizationCardViewModel(const models::OrganizationDTO& dto, QObject* parent)
    : QObject(parent) {
    setFromDTO(dto);
}

void OrganizationCardViewModel::setFromDTO(const models::OrganizationDTO& dto) {
    if (m_organizationId != dto.id) {
        m_organizationId = dto.id;
        emit organizationIdChanged();
    }

    if (m_name != dto.name) {
        m_name = dto.name;
        emit nameChanged();
    }

    QString desc = dto.description.value_or(QString{});
    if (m_description != desc) {
        m_description = desc;
        emit descriptionChanged();
    }

    if (m_city != dto.city.name) {
        m_city = dto.city.name;
        emit cityChanged();
    }

    QString avatarUrl = dto.avatarUrl.has_value() ? dto.avatarUrl.value() : QString{};
    if (m_avatarUrl != avatarUrl) {
        m_avatarUrl = avatarUrl;
        emit avatarUrlChanged();
    }
}

}  // namespace pawspective::viewmodels
