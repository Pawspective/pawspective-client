#include "../../include/viewmodels/update_post_viewmodel.hpp"

namespace pawspective::viewmodels {

UpdatePostViewModel::UpdatePostViewModel(services::PostService& postService, QObject* parent)
    : BaseViewModel(parent), m_postService(postService) {
    setupConnections();
}

void UpdatePostViewModel::setupConnections() {
    connect(&m_postService, &services::PostService::updatePostSuccess, this, &UpdatePostViewModel::handleUpdateSuccess);
    connect(&m_postService, &services::PostService::updatePostFailed, this, &UpdatePostViewModel::handleUpdateFailed);
}

void UpdatePostViewModel::initialize() {
    if (m_postId > 0) {
        emit loadCompleted();
    } else {
        emit loadFailed("Invalid post ID");
    }
}

void UpdatePostViewModel::cleanup() {
    setIsBusy(false);
    discardChanges();
    m_postId = 0;
    m_originalData = models::PostDTO{};
    m_changes = models::PostUpdateDTO{};
}

void UpdatePostViewModel::setText(const QString& value) {
    if (text() != value) {
        m_changes.text = (value == m_originalData.text) ? std::nullopt : std::make_optional(value);
        emit textChanged();
        updateDirtyStatus();
    }
}

void UpdatePostViewModel::setPostData(qint64 postId, const QString& text, const QDateTime& createdAt) {
    m_postId = postId;
    m_originalData.id = postId;
    m_originalData.text = text;
    m_originalData.createdAt = createdAt;
    discardChanges();
}

void UpdatePostViewModel::handleUpdateSuccess(const models::PostDTO& post) {
    setIsBusy(false);
    m_originalData = post;
    m_changes = models::PostUpdateDTO();
    setDirty(false);
    emit saveCompleted();
}

void UpdatePostViewModel::handleUpdateFailed(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);
    if (!error) {
        emitError(ErrorType::UnknownError, "An unexpected error occurred.");
        emit saveFailed("Unknown error");
        return;
    }

    QString message;
    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        message = formatValidationError(validationError);
        emitError(ErrorType::ValidationError, message);
    } else {
        message = error->getMessage();
        emitError(ErrorType::NetworkError, message);
    }

    emit saveFailed(message);
}

void UpdatePostViewModel::saveChanges() {
    if (!m_isDirty || isBusy()) {
        return;
    }

    setIsBusy(true);
    m_postService.updatePost(m_postId, m_changes);
}

void UpdatePostViewModel::discardChanges() {
    m_changes = models::PostUpdateDTO();
    setDirty(false);
    emit textChanged();
}

void UpdatePostViewModel::updateDirtyStatus() {
    bool dirty = m_changes.text.has_value();
    setDirty(dirty);
}

void UpdatePostViewModel::setDirty(bool dirty) {
    updateProperty(m_isDirty, dirty, [this] { emit dirtyChanged(); });
}

}  // namespace pawspective::viewmodels