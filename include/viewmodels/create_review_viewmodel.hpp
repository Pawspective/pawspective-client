#pragma once

#include <functional>
#include <QVariantList>

#include "base.hpp"
#include "models/review_create_dto.hpp"
#include "services/animal_service.hpp"
#include "services/review_service.hpp"

namespace pawspective::viewmodels {

class CreateReviewViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(qint64 animalId READ animalId WRITE setAnimalId NOTIFY animalIdChanged)
    Q_PROPERTY(QVariantList availableAnimals READ availableAnimals NOTIFY availableAnimalsChanged)

public:
    explicit CreateReviewViewModel(
        services::ReviewService& reviewService,
        services::AnimalService& animalService,
        QObject* parent = nullptr
    );

    const QString& text() const { return m_createDto.text; }
    qint64 animalId() const { return m_createDto.animalId; }
    QVariantList availableAnimals() const { return m_availableAnimals; }

    void setText(const QString& value) {
        updateProperty(m_createDto.text, value, [this] { emit textChanged(); });
    }

    void setAnimalId(qint64 value) {
        updateProperty(m_createDto.animalId, value, [this] { emit animalIdChanged(); });
    }

    Q_INVOKABLE void createReview();
    Q_INVOKABLE void loadAvailableAnimals();

    void initialize() override;
    void cleanup() override;

signals:
    void textChanged();
    void animalIdChanged();
    void availableAnimalsChanged();

    void creationFinished(bool success);

private:
    void setupConnections();
    void onAnimalsLoaded(const QList<models::AnimalDTO>& animals);
    void onAnimalsLoadFailed(QSharedPointer<services::BaseError> error);
    void onReviewCreated();
    void onError(QSharedPointer<services::BaseError> error);
    bool validateRequiredFields();

    services::ReviewService& m_reviewService;
    services::AnimalService& m_animalService;
    models::ReviewCreateDTO m_createDto;
    QVariantList m_availableAnimals;
};

}  // namespace pawspective::viewmodels