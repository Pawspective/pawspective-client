#pragma once

#include <QObject>
#include <QString>

#include "base.hpp"
#include "models/animal_dto.hpp"
#include "services/animal_service.hpp"
#include "services/organization_service.hpp"

namespace pawspective::viewmodels {

class AnimalDetailViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString animalType READ animalType NOTIFY animalTypeChanged)
    Q_PROPERTY(QString breedName READ breedName NOTIFY breedNameChanged)
    Q_PROPERTY(qint32 age READ age NOTIFY ageChanged)
    Q_PROPERTY(QString gender READ gender NOTIFY genderChanged)
    Q_PROPERTY(QString size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QString careLevel READ careLevel NOTIFY careLevelChanged)
    Q_PROPERTY(QString color READ color NOTIFY colorChanged)
    Q_PROPERTY(QString goodWith READ goodWith NOTIFY goodWithChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(qint64 organizationId READ organizationId NOTIFY organizationIdChanged)
    Q_PROPERTY(QString organizationName READ organizationName NOTIFY organizationNameChanged)
    Q_PROPERTY(QString organizationCity READ organizationCity NOTIFY organizationCityChanged)
    Q_PROPERTY(QString organizationDescription READ organizationDescription NOTIFY organizationDescriptionChanged)

public:
    explicit AnimalDetailViewModel(
        services::AnimalService& animalService,
        services::OrganizationService& organizationService,
        QObject* parent = nullptr
    );

    const QString& name() const { return m_name; }
    const QString& animalType() const { return m_animalType; }
    const QString& breedName() const { return m_breedName; }
    qint32 age() const { return m_age; }
    const QString& gender() const { return m_gender; }
    const QString& size() const { return m_size; }
    const QString& careLevel() const { return m_careLevel; }
    const QString& color() const { return m_color; }
    const QString& goodWith() const { return m_goodWith; }
    const QString& status() const { return m_status; }
    const QString& description() const { return m_description; }
    qint64 organizationId() const { return m_organizationId; }
    const QString& organizationName() const { return m_organizationName; }
    const QString& organizationCity() const { return m_organizationCity; }
    const QString& organizationDescription() const { return m_organizationDescription; }

    Q_INVOKABLE void loadAnimal(qint64 id);
    void initialize() override {}
    void cleanup() override {}

signals:
    void nameChanged();
    void animalTypeChanged();
    void breedNameChanged();
    void ageChanged();
    void genderChanged();
    void sizeChanged();
    void careLevelChanged();
    void colorChanged();
    void goodWithChanged();
    void statusChanged();
    void descriptionChanged();
    void organizationIdChanged();
    void organizationNameChanged();
    void organizationCityChanged();
    void organizationDescriptionChanged();

private:
    void setFromDTO(const models::AnimalDTO& dto);
    void setFromOrgDTO(const models::OrganizationDTO& dto);

    services::AnimalService& m_animalService;
    services::OrganizationService& m_organizationService;

    QString m_name;
    QString m_animalType;
    QString m_breedName;
    qint32 m_age = 0;
    QString m_gender;
    QString m_size;
    QString m_careLevel;
    QString m_color;
    QString m_goodWith;
    QString m_status;
    QString m_description;
    qint64 m_organizationId = 0;
    QString m_organizationName;
    QString m_organizationCity;
    QString m_organizationDescription;
};

}  // namespace pawspective::viewmodels
