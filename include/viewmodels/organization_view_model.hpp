#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QString>

#include <optional>

#include "models/organization_dto.hpp"
#include "models/user_dto.hpp"
#include "services/auth_service.hpp"
#include "services/organization_service.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

class OrganizationViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(bool hasOrganization READ hasOrganization NOTIFY hasOrganizationChanged)
    Q_PROPERTY(bool canUpdateOrganization READ canUpdateOrganization NOTIFY canUpdateOrganizationChanged)
    Q_PROPERTY(QString organizationName READ organizationName NOTIFY organizationDataChanged)
    Q_PROPERTY(QString organizationCity READ organizationCity NOTIFY organizationDataChanged)
    Q_PROPERTY(QString organizationDescription READ organizationDescription NOTIFY organizationDataChanged)
    Q_PROPERTY(bool showDescription READ showDescription WRITE setShowDescription NOTIFY showDescriptionChanged)
    Q_PROPERTY(int currentTab READ currentTab WRITE setCurrentTab NOTIFY currentTabChanged)
    Q_PROPERTY(qint64 currentOrganizationId READ currentOrganizationId NOTIFY currentOrganizationIdChanged)

public:
    explicit OrganizationViewModel(
        services::AuthService& authService,
        services::OrganizationService& organizationService,
        QObject* parent = nullptr
    );

    bool hasOrganization() const;
    bool canUpdateOrganization() const;
    QString organizationName() const;
    QString organizationCity() const;
    QString organizationDescription() const;
    bool showDescription() const;
    int currentTab() const;
    qint64 currentOrganizationId() const;

    void setShowDescription(bool value);
    void setCurrentTab(int value);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void initializeForOrganization(qint64 organizationId);
    Q_INVOKABLE void initializeForCreateOrganization();
    Q_INVOKABLE void setCanUpdateOrganization(bool value);
    Q_INVOKABLE void refreshOrganization();
    Q_INVOKABLE void createOrganization();
    Q_INVOKABLE void updateOrganization();

signals:
    void hasOrganizationChanged();
    void canUpdateOrganizationChanged();
    void organizationDataChanged();
    void showDescriptionChanged();
    void currentTabChanged();
    void currentOrganizationIdChanged();

    void organizationLoaded();
    void organizationLoadFailed(const QString& errorMessage);
    void createOrganizationRequested();
    void updateOrganizationRequested();
    void sessionExpired();

private slots:
    void handleGetCurrentUserSuccess(const models::UserDTO& user);
    void handleGetCurrentUserFailed(QSharedPointer<services::BaseError> error);
    void handleGetOrganizationSuccess(const models::OrganizationDTO& organization);
    void handleGetOrganizationFailed(QSharedPointer<services::BaseError> error);
    void handleCreateOrganizationSuccess(const models::OrganizationDTO& organization);
    void handleCreateOrganizationFailed(QSharedPointer<services::BaseError> error);
    void handleUpdateOrganizationSuccess(const models::OrganizationDTO& organization);
    void handleUpdateOrganizationFailed(QSharedPointer<services::BaseError> error);
    void handleRefreshFailed(QSharedPointer<services::BaseError> error);
    void handleSessionEnded();
    void handleLoginSuccess(const QString& accessToken, const QString& refreshToken, const QString& tokenType, uint64_t userId);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    services::AuthService& m_authService;
    services::OrganizationService& m_organizationService;

    models::OrganizationDTO m_organizationData;
    qint64 m_currentOrganizationId = 0;
    bool m_hasOrganization = false;
    bool m_canUpdateOrganization = false;
    bool m_showDescription = false;
    int m_currentTab = 1;

    bool tryLoadCurrentOrganization();
    void resetOrganizationState();
    void applyOrganizationLoaded(
        const models::OrganizationDTO& organization,
        bool hasOrganizationValue,
        std::optional<bool> canUpdateOrganizationValue = std::nullopt
    );
    void handleNetworkFailure(QSharedPointer<services::BaseError> error, bool emitLoadFailedSignal);

    void loadOrganizationById(qint64 organizationId);
    void updateOrganizationData(const models::OrganizationDTO& organization);
    void clearOrganizationData();
};

}  // namespace pawspective::viewmodels
