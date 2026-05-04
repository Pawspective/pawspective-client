#include "services/city_service.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>

#include "models/city_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

namespace pawspective::services {

CityService::CityService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void CityService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
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

void CityService::handleSuccess(
    QNetworkReply& reply,
    std::function<void(const QJsonArray&)> onSuccess,
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

        if (!doc.isArray()) {
            onError(QSharedPointer<BaseError>(new UnknownError("Expected JSON array in response")));
            return;
        }

        onSuccess(doc.array());
    } catch (const std::exception& e) {
        onError(QSharedPointer<BaseError>(new ClientJsonParseError(QString(e.what()))));
    }
}

void CityService::getCities() {
    m_networkClient.get(
        QUrl("/city"),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonArray& array) {
                    QList<models::CityDTO> cities;
                    for (const auto& value : array) {
                        cities.append(models::CityDTO::fromJson(value.toObject()));
                    }
                    emit getCitiesSuccess(cities);
                },
                [this](QSharedPointer<BaseError> error) { emit getCitiesFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getCitiesFailed(error); });
        }
    );
}

}  // namespace pawspective::services
