#pragma once
#include "base.hpp"
#include "services/auth_service.hpp"

namespace pawspective::viewmodels {

class LoginViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit LoginViewModel(services::AuthService& authService, QObject* parent = nullptr);

    QString email() const { return m_email; }

    void setEmail(const QString& value) {
        updateProperty(m_email, value, [this] { emit emailChanged(); });
    }

    QString password() const { return m_password; }

    void setPassword(const QString& value) {
        updateProperty(m_password, value, [this] { emit passwordChanged(); });
    }

    Q_INVOKABLE void login();

    void initialize() override {}
    void cleanup() override {}

signals:
    void emailChanged();
    void passwordChanged();
    void loginFinished(bool success);

private:
    services::AuthService& m_authService;
    QString m_email;     // NOLINT(misc-non-private-member-variables-in-classes)
    QString m_password;  // NOLINT(misc-non-private-member-variables-in-classes)
};
}  // namespace pawspective::viewmodels