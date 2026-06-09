#include "../../include/viewmodels/create_post_viewmodel.hpp"

#include "services/errors.hpp"

namespace pawspective::viewmodels {

CreatePostViewModel::CreatePostViewModel(services::PostService& postService, QObject* parent)
    : BaseViewModel(parent), m_postService(postService) {
    setupConnections();
}

void CreatePostViewModel::setupConnections() {
    connect(&m_postService, &services::PostService::createPostSuccess, this, &CreatePostViewModel::onPostCreated);

    connect(&m_postService, &services::PostService::createPostFailed, this, &CreatePostViewModel::onError);
}

bool CreatePostViewModel::validateRequiredFields() {
    if (m_createDto.text.trimmed().isEmpty()) {
        emitError(ValidationError, "Please enter post content.");
        return false;
    }

    return true;
}

void CreatePostViewModel::onPostCreated() {
    setIsBusy(false);
    emit creationFinished(true);
}

void CreatePostViewModel::onError(QSharedPointer<services::BaseError> error) {
    setIsBusy(false);

    if (const auto& validationError = error.dynamicCast<services::ValidationError>()) {
        emitError(ValidationError, formatValidationError(validationError));
    } else {
        emitError(NetworkError, error->getMessage());
    }

    emit creationFinished(false);
}

void CreatePostViewModel::createPost() {
    if (!validateRequiredFields()) {
        emit creationFinished(false);
        return;
    }

    if (m_organizationId <= 0) {
        emitError(NetworkError, "Organization ID is not set.");
        emit creationFinished(false);
        return;
    }
    setIsBusy(true);
    m_postService.createPost(m_createDto);
}

void CreatePostViewModel::addPhoto(const QString& fileName) {
    if (m_createDto.photos.size() >= 10) return;
    if (!m_createDto.photos.contains(fileName)) {
        m_createDto.photos.append(fileName);
        emit photosChanged();
    }
}

void CreatePostViewModel::removePhoto(const QString& fileName) {
    if (m_createDto.photos.removeAll(fileName) > 0) {
        emit photosChanged();
    }
}

void CreatePostViewModel::initialize() {}

void CreatePostViewModel::cleanup() {
    updateProperty(m_createDto.text, QString(), [this] { emit textChanged(); });
    if (!m_createDto.photos.isEmpty()) {
        m_createDto.photos.clear();
        emit photosChanged();
    }
    m_organizationId = 0;
    setIsBusy(false);
}
}  // namespace pawspective::viewmodels