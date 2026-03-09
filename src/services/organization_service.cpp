#include "services/organization_service.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>

#include "models/organization_dto.hpp"
#include "services/errors.hpp"

namespace pawspective::services {

OrganizationService::OrganizationService(NetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void OrganizationService::handleError(QNetworkReply& reply) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit requestFailed(
            QSharedPointer<BaseError>(new ClientJsonParseError(
                QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
            ))
        );
        return;
    }

    if (doc.isObject()) {
        auto error = ErrorFactory::createError(doc.object());
        emit requestFailed(QSharedPointer<BaseError>(std::move(error)));
    } else {
        emit requestFailed(QSharedPointer<BaseError>(new UnknownError("Unknown error occurred")));
    }
}

void OrganizationService::handleSuccess(QNetworkReply& reply, std::function<void(const QJsonObject&)> onSuccess) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
    try {
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            emit requestFailed(
                QSharedPointer<BaseError>(new ClientJsonParseError(
                    QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
                ))
            );
            return;
        }

        onSuccess(doc.object());
    } catch (const std::exception& e) {
        emit requestFailed(QSharedPointer<BaseError>(new ClientJsonParseError(QString(e.what()))));
    }
}

void OrganizationService::getOrganization(qint64 id) {
    m_networkClient.get(
        QUrl(QString("/orgs/%1").arg(id)),
        [this](QNetworkReply& reply) {
            handleSuccess(reply, [this](const QJsonObject& obj) {
                models::OrganizationDTO organization = models::OrganizationDTO::fromJson(obj);
                emit getOrganizationSuccess(organization);
            });
        },
        [this](QNetworkReply& reply) { handleError(reply); }
    );
}

void OrganizationService::createOrganization(const models::OrganizationRegisterDTO& dto) {
    const QJsonDocument doc(dto.toJson());
    m_networkClient.post(
        QUrl("/orgs"),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(reply, [this](const QJsonObject& obj) {
                models::OrganizationDTO organization = models::OrganizationDTO::fromJson(obj);
                emit createOrganizationSuccess(organization);
            });
        },
        [this](QNetworkReply& reply) { handleError(reply); }
    );
}

void OrganizationService::updateOrganization(qint64 id, const models::OrganizationUpdateDTO& dto) {
    const QJsonDocument doc(dto.toJson());
    m_networkClient.put(
        QUrl(QString("/orgs/%1").arg(id)),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(reply, [this](const QJsonObject& obj) {
                models::OrganizationDTO organization = models::OrganizationDTO::fromJson(obj);
                emit updateOrganizationSuccess(organization);
            });
        },
        [this](QNetworkReply& reply) { handleError(reply); }
    );
}

}  // namespace pawspective::services
