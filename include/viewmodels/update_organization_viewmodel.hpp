#pragma once

#include <QVariantList>
#include "models/organization_dto.hpp"
#include "models/organization_update_dto.hpp"
#include "models/user_dto.hpp"
#include "services/auth_service.hpp"
#include "services/city_service.hpp"
#include "services/organization_service.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

class UpdateOrganizationViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(qint64 cityId READ cityId WRITE setCityId NOTIFY cityIdChanged)
    Q_PROPERTY(QVariantList cities READ cities NOTIFY citiesChanged)
    Q_PROPERTY(int currentCityIndex READ currentCityIndex NOTIFY currentCityIndexChanged)
    Q_PROPERTY(bool isDirty READ isDirty NOTIFY dirtyChanged)

public:
    explicit UpdateOrganizationViewModel(
        services::OrganizationService& organizationService,
        services::CityService& cityService,
        services::AuthService& authService,
        QObject* parent = nullptr
    );

    QString name() const;
    QString description() const;
    qint64 cityId() const;
    const QVariantList& cities() const { return m_cities; }
    int currentCityIndex() const;
    bool isDirty() const { return m_isDirty; }

    void setName(const QString& value);
    void setDescription(const QString& value);
    void setCityId(qint64 value);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void discardChanges();

signals:
    void nameChanged();
    void descriptionChanged();
    void cityIdChanged();
    void citiesChanged();
    void currentCityIndexChanged();
    void dirtyChanged();

    void loadCompleted();
    void loadFailed(const QString& errorMessage);
    void saveCompleted();
    void saveFailed(const QString& errorMessage);

private slots:
    void handleGetSuccess(const models::OrganizationDTO& organization);
    void handleUpdateSuccess(const models::OrganizationDTO& organization);
    void handleCitiesSuccess(const QList<models::CityDTO>& cities);
    void handleGetCurrentUserSuccess(const models::UserDTO& user);
    void handleGetFailed(QSharedPointer<services::BaseError> error);
    void handleUpdateFailed(QSharedPointer<services::BaseError> error);
    void handleCitiesFailed(QSharedPointer<services::BaseError> error);
    void handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    void updateDirtyStatus();
    void notifyAllChanged();
    void setDirty(bool dirty);

    services::OrganizationService& m_organizationService;
    services::CityService& m_cityService;
    services::AuthService& m_authService;

    models::OrganizationDTO m_originalData;
    models::OrganizationUpdateDTO m_changes;
    QVariantList m_cities;
    bool m_isDirty;
};

}  // namespace pawspective::viewmodels