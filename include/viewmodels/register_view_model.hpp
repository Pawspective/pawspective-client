#pragma once
#include "base.hpp"
#include "services/user_service.hpp"

namespace pawspective::viewmodels {
class RegisterViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString confirmPassword READ confirmPassword WRITE setConfirmPassword NOTIFY confirmPasswordChanged)

public:
    explicit RegisterViewModel(services::UserService& userService, QObject* parent = nullptr);

    const QString& firstName() const { return m_firstName; }

    void setFirstName(const QString& value) {
        updateProperty(m_firstName, value, [this] { emit firstNameChanged(); });
    }

    const QString& lastName() const { return m_lastName; }

    void setLastName(const QString& value) {
        updateProperty(m_lastName, value, [this] { emit lastNameChanged(); });
    }

    const QString& email() const { return m_email; }

    void setEmail(const QString& value) {
        updateProperty(m_email, value, [this] { emit emailChanged(); });
    }

    const QString& password() const { return m_password; }

    void setPassword(const QString& value) {
        updateProperty(m_password, value, [this] { emit passwordChanged(); });
    }

    const QString& confirmPassword() const { return m_confirmPassword; }

    void setConfirmPassword(const QString& value) {
        updateProperty(m_confirmPassword, value, [this] { emit confirmPasswordChanged(); });
    }

    Q_INVOKABLE void registerUser();

    void initialize() override {}
    void cleanup() override {}

signals:
    void firstNameChanged();
    void lastNameChanged();
    void emailChanged();
    void passwordChanged();
    void confirmPasswordChanged();
    void registrationFinished(bool success);

private:
    services::UserService& m_userService;
    QString m_firstName;        // NOLINT(misc-non-private-member-variables-in-classes)
    QString m_lastName;         // NOLINT(misc-non-private-member-variables-in-classes)
    QString m_email;            // NOLINT(misc-non-private-member-variables-in-classes)
    QString m_password;         // NOLINT(misc-non-private-member-variables-in-classes)
    QString m_confirmPassword;  // NOLINT(misc-non-private-member-variables-in-classes)
};
}  // namespace pawspective::viewmodels