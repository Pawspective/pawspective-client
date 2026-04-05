#pragma once

#include <QObject>
#include <QString>

#include "models/animal_dto.hpp"

namespace pawspective::viewmodels {

class AnimalDetailViewModel : public QObject {
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

public:
    explicit AnimalDetailViewModel(QObject* parent = nullptr);
    explicit AnimalDetailViewModel(const models::AnimalDTO& dto, QObject* parent = nullptr);

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

    Q_INVOKABLE void setFromDTO(const models::AnimalDTO& dto);

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

private:
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
};

}  // namespace pawspective::viewmodels
