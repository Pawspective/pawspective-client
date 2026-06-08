// photo_service.cpp
#include "services/photo_service.hpp"

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMimeDatabase>
#include <QMimeType>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>

#include "services/errors.hpp"

namespace pawspective::services {

PhotoService::PhotoService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void PhotoService::uploadPhoto(const QString& filePath) {
    qDebug() << "PhotoService::uploadPhoto called with:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << filePath;
        emit uploadPhotoFailed(QSharedPointer<UnknownError>::create(QString("Cannot open file: %1").arg(filePath)));
        return;
    }

    const QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        qDebug() << "File is empty:" << filePath;
        emit uploadPhotoFailed(QSharedPointer<UnknownError>::create("File is empty"));
        return;
    }

    QMimeDatabase mimeDb;
    const QString contentType = mimeDb.mimeTypeForFile(filePath).name();
    qDebug() << "Content type:" << contentType;
    qDebug() << "File size:" << data.size();

    m_networkClient.postRaw(
        QUrl("/upload/photo"),
        data,
        contentType,
        [this](QNetworkReply& reply) {
            qDebug() << "PhotoService: upload success callback triggered";
            const QByteArray responseData = reply.property("responseData").toByteArray();
            qDebug() << "Response data:" << responseData;

            QJsonParseError parseError;
            const QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

            if (parseError.error != QJsonParseError::NoError) {
                qDebug() << "JSON parse error:" << parseError.errorString();
                emit uploadPhotoFailed(
                    QSharedPointer<ClientJsonParseError>::create(QString("JSON parse error: %1").arg(parseError.errorString()))
                );
                return;
            }

            const QJsonObject obj = doc.object();
            qDebug() << "Response object keys:" << obj.keys();

            if (!obj.contains("filename") || obj["filename"].toString().isEmpty()) {
                qDebug() << "Missing filename in response";
                emit uploadPhotoFailed(QSharedPointer<UnknownError>::create("Missing 'filename' in response"));
                return;
            }

            QString fileName = obj["filename"].toString();
            qDebug() << "Emitting uploadPhotoSuccess with fileName:" << fileName;
            emit uploadPhotoSuccess(fileName);
        },
        [this](QNetworkReply& reply) {
            qDebug() << "PhotoService: upload error callback triggered";
            const QByteArray responseData = reply.property("responseData").toByteArray();
            qDebug() << "Error response:" << responseData;
            if (responseData.isEmpty()) {
                emit uploadPhotoFailed(QSharedPointer<UnknownError>::create("Empty error response"));
                return;
            }
            QJsonParseError parseError;
            const QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
            if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
                emit uploadPhotoFailed(QSharedPointer<UnknownError>::create(QString::fromUtf8(responseData)));
                return;
            }
            emit uploadPhotoFailed(QSharedPointer<BaseError>(ErrorFactory::createError(doc.object())));
        }
    );
}

void PhotoService::getPhoto(const QString& photoUrl) {
    m_networkClient.get(
        QUrl(photoUrl),
        [this](QNetworkReply& reply) {
            const QByteArray data = reply.property("responseData").toByteArray();
            QImage image;
            if (!image.loadFromData(data)) {
                emit getPhotoFailed(QSharedPointer<UnknownError>::create("Failed to decode image data"));
                return;
            }
            emit getPhotoSuccess(image);
        },
        [this](QNetworkReply& reply) {
            const QByteArray data = reply.property("responseData").toByteArray();
            emit getPhotoFailed(
                QSharedPointer<UnknownError>::create(data.isEmpty() ? "Failed to fetch photo" : QString::fromUtf8(data))
            );
        }
    );
}

} // namespace pawspective::services