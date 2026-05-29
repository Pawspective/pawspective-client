#include "services/post_service.hpp"

#include <QJsonArray>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QUrlQuery>

#include "models/post_dto.hpp"
#include "validator.hpp"

namespace pawspective::services {
PostService::PostService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void PostService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
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

void PostService::handleSuccess(
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

void PostService::getByOrganizationId(qint64 id) {
    QUrlQuery query;
    query.addQueryItem("org_id", QString::number(id));

    QUrl url("/posts");
    url.setQuery(query);
    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) { emit getByOrganizationIdSuccess(models::PostListDTO::fromJson(obj)); },
                [this](QSharedPointer<BaseError> error) { emit getByOrganizationIdFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getByOrganizationIdFailed(error); });
        }
    );
}

void PostService::createPost(const models::PostCreateDTO& dto) {
    utils::Validator validator;
    validator.field("text", dto.text.toStdString()).notBlank().maxLength(2000);
    if (auto error = validator.getValidationError()) {
        emit createPostFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
    const QJsonDocument data(dto.toJson());
    m_networkClient.post(
        QUrl("/posts"),
        data.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) { emit createPostSuccess(models::PostDTO::fromJson(obj)); },
                [this](QSharedPointer<BaseError> error) { emit createPostFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit createPostFailed(error); });
        }
    );
}

void PostService::updatePost(qint64 id, const models::PostUpdateDTO& dto) {
    utils::Validator validator;
    if (dto.text) {
        validator.field("text", dto.text->toStdString()).notBlank().maxLength(2000);
    }
    if (auto error = validator.getValidationError()) {
        emit updatePostFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
    const QJsonDocument data(dto.toJson());
    m_networkClient.put(
        QUrl(QString("/posts/%1").arg(id)),
        data.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) { emit updatePostSuccess(models::PostDTO::fromJson(obj)); },
                [this](QSharedPointer<BaseError> error) { emit updatePostFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit updatePostFailed(error); });
        }
    );
}

void PostService::deletePost(qint64 id) {
    m_networkClient.deleteResource(
        QUrl(QString("/posts/%1").arg(id)),
        [this](QNetworkReply& /*reply*/) { emit deletePostSuccess(); },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit deletePostFailed(error); });
        }
    );
}

}  // namespace pawspective::services
