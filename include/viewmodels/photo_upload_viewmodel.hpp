#pragma once

#include <QObject>
#include <QString>
#include "services/photo_service.hpp"
#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {

class PhotoUploadViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(bool isUploading READ isUploading NOTIFY isUploadingChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QString uploadedFileName READ uploadedFileName NOTIFY uploadedFileNameChanged)

public:
    explicit PhotoUploadViewModel(services::PhotoService& photoService, QObject* parent = nullptr);

    bool isUploading() const { return m_isUploading; }
    const QString& errorMessage() const { return m_errorMessage; }
    const QString& uploadedFileName() const { return m_uploadedFileName; }

    Q_INVOKABLE void uploadPhoto(const QString& filePath);
    Q_INVOKABLE void clear();

    void initialize() override;
    void cleanup() override;

signals:
    void isUploadingChanged();
    void errorMessageChanged();
    void uploadedFileNameChanged();
    void uploadCompleted(const QString& fileName);
    void uploadFailed(const QString& errorMessage);

private slots:
    void onUploadSuccess(const QString& fileName);
    void onUploadFailed(QSharedPointer<services::BaseError> error);

    // NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    void setIsUploading(bool value);
    void setErrorMessage(const QString& value);
    void setUploadedFileName(const QString& value);

    services::PhotoService& m_photoService;
    bool m_isUploading = false;
    QString m_errorMessage;
    QString m_uploadedFileName;
};

}  // namespace pawspective::viewmodels