#include "services/breed_service.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>
#include <QUrlQuery>

#include "models/animal_enums.hpp"
#include "models/breed_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "validator.hpp"

namespace pawspective::services {

BreedService::BreedService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void BreedService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
    QJsonParseError parseError;
    QByteArray data = reply.property("responseData").toByteArray();
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (data.isEmpty()) {
        onError(QSharedPointer<UnknownError>::create("Network error. Check your internet connection."));
        return;
    }

    if (parseError.error != QJsonParseError::NoError) {
        onError(
            QSharedPointer<BaseError>(new ClientJsonParseError(
                QString("JSON parse error at %1: %2").arg(parseError.offset).arg(parseError.errorString())
            ))
        );
        return;
    }

    if (doc.isObject()) {
        auto error = ErrorFactory::createError(doc.object());
        onError(QSharedPointer<BaseError>(std::move(error)));
    } else {
        onError(QSharedPointer<BaseError>(new UnknownError("Unknown error occurred")));
    }
}

void BreedService::handleSuccess(
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

void BreedService::getBreedsByType(models::AnimalType type) {
    utils::Validator validator;
    validator.field("type", models::toApiString(type).toStdString()).notBlank();
    if (auto error = validator.getValidationError()) {
        emit getBreedsByTypeFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    QUrl url("/breeds");
    QUrlQuery query;
    query.addQueryItem("type", models::toApiString(type));
    url.setQuery(query);

    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonArray& array) {
                    QList<models::BreedDTO> breeds;
                    for (const auto& value : array) {
                        breeds.append(models::BreedDTO::fromJson(value.toObject()));
                    }
                    emit getBreedsByTypeSuccess(breeds);
                },
                [this](QSharedPointer<BaseError> error) { emit getBreedsByTypeFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getBreedsByTypeFailed(error); });
        }
    );
}

}  // namespace pawspective::services
