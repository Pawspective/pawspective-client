#include "services/review_service.hpp"

#include <QJsonArray>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QUrlQuery>

#include "models/review_dto.hpp"
#include "validator.hpp"

namespace pawspective::services {
ReviewService::ReviewService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void ReviewService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
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

void ReviewService::handleSuccess(
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

void ReviewService::getByOrganizationId(qint64 id, int page, int limit) {
    QUrlQuery query;
    query.addQueryItem("org_id", QString::number(id));
    if (page > 0) {
        query.addQueryItem("page", QString::number(page));
    }
    if (limit > 0) {
        query.addQueryItem("limit", QString::number(limit));
    }

    QUrl url("/reviews");
    url.setQuery(query);
    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    emit getByOrganizationIdSuccess(models::ReviewListDTO::fromJson(obj));
                },
                [this](QSharedPointer<BaseError> error) { emit getByOrganizationIdFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getByOrganizationIdFailed(error); });
        }
    );
}

void ReviewService::createReview(const models::ReviewCreateDTO& dto) {
    utils::Validator validator;
    validator.field("text", dto.text.toStdString()).notBlank().maxLength(2000);
    if (auto error = validator.getValidationError()) {
        emit createReviewFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
    const QJsonDocument data(dto.toJson());
    m_networkClient.post(
        QUrl("/reviews"),
        data.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) { emit createReviewSuccess(models::ReviewDTO::fromJson(obj)); },
                [this](QSharedPointer<BaseError> error) { emit createReviewFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit createReviewFailed(error); });
        }
    );
}

void ReviewService::updateReview(qint64 id, const models::ReviewUpdateDTO& dto) {
    utils::Validator validator;
    if (dto.text) {
        validator.field("text", dto.text->toStdString()).notBlank().maxLength(2000);
    }
    if (auto error = validator.getValidationError()) {
        emit updateReviewFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
    const QJsonDocument data(dto.toJson());
    m_networkClient.put(
        QUrl(QString("/reviews/%1").arg(id)),
        data.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) { emit updateReviewSuccess(models::ReviewDTO::fromJson(obj)); },
                [this](QSharedPointer<BaseError> error) { emit updateReviewFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit updateReviewFailed(error); });
        }
    );
}

void ReviewService::deleteReview(qint64 id) {
    m_networkClient.deleteResource(
        QUrl(QString("/reviews/%1").arg(id)),
        [this](QNetworkReply& /*reply*/) { emit deleteReviewSuccess(); },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit deleteReviewFailed(error); });
        }
    );
}

}  // namespace pawspective::services
