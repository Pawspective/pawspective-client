#pragma once

#include <QImage>
#include <QObject>
#include <QSharedPointer>

#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

class PhotoService : public QObject {
    Q_OBJECT
public:
    explicit PhotoService(INetworkClient& networkClient, QObject* parent = nullptr);

    void uploadPhoto(const QString& filePath);
    void getPhoto(const QString& photoUrl);

signals:
    void uploadPhotoSuccess(const QString& url);
    void uploadPhotoFailed(QSharedPointer<services::BaseError> error);
    void getPhotoSuccess(const QImage& image);
    void getPhotoFailed(QSharedPointer<services::BaseError> error);

private:
    INetworkClient& m_networkClient;
};

}  // namespace pawspective::services
