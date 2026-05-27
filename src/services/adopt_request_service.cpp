#include "services/adopt_request_service.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>
#include <QUrlQuery>

#include "models/adopt_request_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

AdoptRequestService::AdoptRequestService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void AdoptRequestService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
    QByteArray data = reply.property("responseData").toByteArray();

    if (data.isEmpty()) {
        onError(QSharedPointer<UnknownError>::create("Empty response"));
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        onError(QSharedPointer<UnknownError>::create(QString::fromUtf8(data)));
        return;
    }

    if (doc.isObject()) {
        auto error = ErrorFactory::createError(doc.object());
        onError(QSharedPointer<BaseError>(std::move(error)));
    } else {
        onError(QSharedPointer<BaseError>(new UnknownError("Unknown error occurred")));
    }
}

void AdoptRequestService::handleSuccess(
    QNetworkReply& reply,
    std::function<void(const QJsonObject&)> onSuccess,
    std::function<void(QSharedPointer<BaseError>)> onError
) {
    QJsonParseError parseError;
    QByteArray data = reply.property("responseData").toByteArray();
    try {
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            onError(
                QSharedPointer<BaseError>(new ClientJsonParseError(
                    QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
                ))
            );
            return;
        }

        onSuccess(doc.object());
    } catch (const std::exception& e) {
        onError(QSharedPointer<BaseError>(new ClientJsonParseError(QString(e.what()))));
    }
}

void AdoptRequestService::createAdoptRequest(qint64 animalId) {
    m_networkClient.post(
        QUrl(QString("/animals/%1/adopt").arg(animalId)),
        {},
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    emit createAdoptRequestSuccess(models::AdoptRequestDTO::fromJson(obj));
                },
                [this](QSharedPointer<BaseError> error) { emit createAdoptRequestFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit createAdoptRequestFailed(error); });
        }
    );
}

void AdoptRequestService::getAdoptRequests(qint64 orgId, int page) {
    QUrl url("/requests");
    QUrlQuery query;
    query.addQueryItem("org_id", QString::number(orgId));
    query.addQueryItem("page", QString::number(page));
    url.setQuery(query);

    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    emit getAdoptRequestsSuccess(models::AdoptRequestListDTO::fromJson(obj));
                },
                [this](QSharedPointer<BaseError> error) { emit getAdoptRequestsFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getAdoptRequestsFailed(error); });
        }
    );
}

void AdoptRequestService::acceptAdoptRequest(qint64 id) {
    m_networkClient.post(
        QUrl(QString("/requests/accept/%1").arg(id)),
        {},
        [this](QNetworkReply& /*reply*/) { emit acceptAdoptRequestSuccess(); },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit acceptAdoptRequestFailed(error); });
        }
    );
}

void AdoptRequestService::denyAdoptRequest(qint64 id) {
    m_networkClient.post(
        QUrl(QString("/requests/deny/%1").arg(id)),
        {},
        [this](QNetworkReply& /*reply*/) { emit denyAdoptRequestSuccess(); },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit denyAdoptRequestFailed(error); });
        }
    );
}

}  // namespace pawspective::services
