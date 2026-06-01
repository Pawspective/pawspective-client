#include "viewmodels/update_review_viewmodel.hpp"

namespace pawspective::viewmodels {

UpdateReviewViewModel::UpdateReviewViewModel(services::ReviewService& reviewService, QObject* parent)
    : BaseViewModel(parent), m_reviewService(reviewService) {
    setupConnections();
}

void UpdateReviewViewModel::setupConnections() {
    connect(
        &m_reviewService,
        &services::ReviewService::updateReviewSuccess,
        this,
        &UpdateReviewViewModel::handleUpdateSuccess
    );
    connect(
        &m_reviewService,
        &services::ReviewService::updateReviewFailed,
        this,
        &UpdateReviewViewModel::handleUpdateFailed
    );
}

void UpdateReviewViewModel::initialize() {
    if (m_reviewId > 0) {
        emit loadCompleted();
    } else {
        emit loadFailed("Invalid review ID");
    }
}

void UpdateReviewViewModel::cleanup() {
    setIsBusy(false);
    discardChanges();
    m_reviewId = 0;
    m_originalData = models::ReviewDTO{};
    m_changes = models::ReviewUpdateDTO{};
}

void UpdateReviewViewModel::setText(const QString& value) {
    if (text() != value) {
        m_changes.text = (value == m_originalData.text) ? std::nullopt : std::make_optional(value);
        emit textChanged();
        updateDirtyStatus();
    }
}

void UpdateReviewViewModel::setReviewData(qint64 reviewId, const QString& text) {
    m_reviewId = reviewId;
    m_originalData.id = reviewId;
    m_originalData.text = text;
    discardChanges();
}

void UpdateReviewViewModel::handleUpdateSuccess(const models::ReviewDTO& review) {
    setIsBusy(false);
    m_originalData = review;
    m_changes = models::ReviewUpdateDTO();
    setDirty(false);
    emit saveCompleted();
}

void UpdateReviewViewModel::handleUpdateFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (!error) {
        emitError(UnknownError, "An unexpected error occurred.");
        emit saveFailed("Unknown error");
        return;
    }

    QString message;
    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        message = formatValidationError(validationError);
        emitError(ValidationError, message);
    } else {
        message = error->getMessage();
        emitError(NetworkError, message);
    }

    emit saveFailed(message);
}

void UpdateReviewViewModel::saveChanges() {
    if (!m_isDirty || isBusy()) {
        return;
    }

    setIsBusy(true);
    m_reviewService.updateReview(m_reviewId, m_changes);
}

void UpdateReviewViewModel::discardChanges() {
    m_changes = models::ReviewUpdateDTO{};
    setDirty(false);
    emit textChanged();
}

void UpdateReviewViewModel::updateDirtyStatus() { setDirty(m_changes.text.has_value()); }

void UpdateReviewViewModel::setDirty(bool dirty) {
    updateProperty(m_isDirty, dirty, [this] { emit dirtyChanged(); });
}

}  // namespace pawspective::viewmodels