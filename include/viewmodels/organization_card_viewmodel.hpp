#pragma once

#include <QObject>
#include <QString>

#include "models/organization_dto.hpp"

namespace pawspective::viewmodels {

class OrganizationCardViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(qint64 organizationId READ organizationId NOTIFY organizationIdChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString city READ city NOTIFY cityChanged)

public:
    explicit OrganizationCardViewModel(QObject* parent = nullptr);
    explicit OrganizationCardViewModel(const models::OrganizationDTO& dto, QObject* parent = nullptr);

    qint64 organizationId() const { return m_organizationId; }
    const QString& name() const { return m_name; }
    const QString& description() const { return m_description; }
    const QString& city() const { return m_city; }

    Q_INVOKABLE void setFromDTO(const models::OrganizationDTO& dto);

signals:
    void organizationIdChanged();
    void nameChanged();
    void descriptionChanged();
    void cityChanged();

private:
    qint64 m_organizationId = 0;
    QString m_name;
    QString m_description;
    QString m_city;
};

}  // namespace pawspective::viewmodels
