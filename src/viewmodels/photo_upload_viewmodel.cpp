// photo_upload_viewmodel.cpp
#include "viewmodels/photo_upload_viewmodel.hpp"

#include <QDebug>

namespace pawspective::viewmodels {

PhotoUploadViewModel::PhotoUploadViewModel(services::PhotoService& photoService, QObject* parent)
    : BaseViewModel(parent), m_photoService(photoService) {

    qDebug() << "PhotoUploadViewModel constructor";

    bool connected1 = connect(&m_photoService, &services::PhotoService::uploadPhotoSuccess,
            this, &PhotoUploadViewModel::onUploadSuccess);
    bool connected2 = connect(&m_photoService, &services::PhotoService::uploadPhotoFailed,
            this, &PhotoUploadViewModel::onUploadFailed);

    qDebug() << "Connected to uploadPhotoSuccess:" << connected1;
    qDebug() << "Connected to uploadPhotoFailed:" << connected2;
}

void PhotoUploadViewModel::initialize() { clear(); }

void PhotoUploadViewModel::cleanup() {
    clear();
    setIsUploading(false);
}

void PhotoUploadViewModel::uploadPhoto(const QString& filePath) {
    qDebug() << "PhotoUploadViewModel::uploadPhoto called with:" << filePath;

    if (filePath.isEmpty()) {
        setErrorMessage("No file selected");
        emit uploadFailed("No file selected");
        return;
    }

    if (m_isUploading) {
        qDebug() << "Already uploading, ignoring";
        return;
    }

    clear();
    setIsUploading(true);
    qDebug() << "Calling m_photoService.uploadPhoto";
    m_photoService.uploadPhoto(filePath);
}

void PhotoUploadViewModel::clear() {
    setErrorMessage(QString());
    setUploadedFileName(QString());
}

void PhotoUploadViewModel::onUploadSuccess(const QString& fileName) {
    qDebug() << "=== PhotoUploadViewModel::onUploadSuccess - fileName:" << fileName;
    setIsUploading(false);
    setUploadedFileName(fileName);
    emit uploadCompleted(fileName);
    qDebug() << "uploadCompleted signal emitted";
}

void PhotoUploadViewModel::onUploadFailed(QSharedPointer<services::BaseError> error) {
    qDebug() << "PhotoUploadViewModel::onUploadFailed - error:" << (error ? error->getMessage() : "unknown");
    setIsUploading(false);
    QString message = error ? error->getMessage() : "Upload failed";
    setErrorMessage(message);
    emit uploadFailed(message);
}

void PhotoUploadViewModel::setIsUploading(bool value) {
    if (m_isUploading != value) {
        m_isUploading = value;
        emit isUploadingChanged();
    }
}

void PhotoUploadViewModel::setErrorMessage(const QString& value) {
    if (m_errorMessage != value) {
        m_errorMessage = value;
        emit errorMessageChanged();
    }
}

void PhotoUploadViewModel::setUploadedFileName(const QString& value) {
    if (m_uploadedFileName != value) {
        m_uploadedFileName = value;
        emit uploadedFileNameChanged();
    }
}

} // namespace pawspective::viewmodels