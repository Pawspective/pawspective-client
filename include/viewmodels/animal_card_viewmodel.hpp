#pragma once

#include <QObject>
#include <QString>

#include "models/animal_dto.hpp"

namespace pawspective::viewmodels {

class AnimalCardViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(qint32 age READ age NOTIFY ageChanged)
    Q_PROPERTY(QString animalType READ animalType NOTIFY animalTypeChanged)

public:
    explicit AnimalCardViewModel(QObject* parent = nullptr);
    explicit AnimalCardViewModel(const models::AnimalDTO& dto, QObject* parent = nullptr);

    const QString& name() const { return m_name; }
    const QString& description() const { return m_description; }
    qint32 age() const { return m_age; }
    const QString& animalType() const { return m_animalType; }

    Q_INVOKABLE void setFromDTO(const models::AnimalDTO& dto);

signals:
    void nameChanged();
    void descriptionChanged();
    void ageChanged();
    void animalTypeChanged();

private:
    QString m_name;
    QString m_description;
    qint32 m_age = 0;
    QString m_animalType;
};

}  // namespace pawspective::viewmodels
