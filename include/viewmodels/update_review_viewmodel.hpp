#pragma once

#include <optional>

#include "base.hpp"
#include "models/review_dto.hpp"
#include "models/review_update_dto.hpp"
#include "services/review_service.hpp"

namespace pawspective::viewmodels {

class UpdateReviewViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool isDirty READ isDirty NOTIFY dirtyChanged)

public:
    explicit UpdateReviewViewModel(services::ReviewService& reviewService, QObject* parent = nullptr);

    QString text() const { return m_changes.text.value_or(m_originalData.text); }
    bool isDirty() const { return m_isDirty; }

    void setText(const QString& value);

    Q_INVOKABLE void initialize() override;
    Q_INVOKABLE void cleanup() override;
    Q_INVOKABLE void saveChanges();
    Q_INVOKABLE void discardChanges();
    Q_INVOKABLE void setReviewData(qint64 reviewId, const QString& text);

signals:
    void textChanged();
    void dirtyChanged();

    void loadCompleted();
    void loadFailed(const QString& errorMessage);
    void saveCompleted();
    void saveFailed(const QString& errorMessage);

private slots:
    void handleUpdateSuccess(const models::ReviewDTO& review);
    void handleUpdateFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    void setupConnections();
    void updateDirtyStatus();
    void setDirty(bool dirty);

    services::ReviewService& m_reviewService;
    qint64 m_reviewId = 0;

    models::ReviewDTO m_originalData;
    models::ReviewUpdateDTO m_changes;
    bool m_isDirty = false;
};

}  // namespace pawspective::viewmodels