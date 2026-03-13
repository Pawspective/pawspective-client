#pragma once
#include <QVariantList>

#include "base.hpp"
#include "services/city_service.hpp"
#include "services/organization_service.hpp"

namespace pawspective::viewmodels {

class RegisterOrganizationViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(qint64 cityId READ cityId WRITE setCityId NOTIFY cityIdChanged)
    Q_PROPERTY(QVariantList cities READ cities NOTIFY citiesChanged)

public:
    explicit RegisterOrganizationViewModel(
        services::OrganizationService& organizationService,
        services::CityService& cityService,
        QObject* parent = nullptr
    );

    const QString& name() const { return m_name; }

    void setName(const QString& value) {
        updateProperty(m_name, value, [this] { emit nameChanged(); });
    }

    const QString& description() const { return m_description; }

    void setDescription(const QString& value) {
        updateProperty(m_description, value, [this] { emit descriptionChanged(); });
    }

    qint64 cityId() const { return m_cityId; }

    void setCityId(qint64 value) {
        updateProperty(m_cityId, value, [this] { emit cityIdChanged(); });
    }

    const QVariantList& cities() const { return m_cities; }

    Q_INVOKABLE void registerOrganization();
    Q_INVOKABLE void loadCities();

    void initialize() override;
    void cleanup() override {}

signals:
    void nameChanged();
    void descriptionChanged();
    void cityIdChanged();
    void citiesChanged();
    void registrationFinished(bool success);

private:
    services::OrganizationService& m_organizationService;
    services::CityService& m_cityService;
    QString m_name;         // NOLINT(misc-non-private-member-variables-in-classes)
    QString m_description;  // NOLINT(misc-non-private-member-variables-in-classes)
    qint64 m_cityId = 0;    // NOLINT(misc-non-private-member-variables-in-classes)
    QVariantList m_cities;  // NOLINT(misc-non-private-member-variables-in-classes)
};

}  // namespace pawspective::viewmodels
