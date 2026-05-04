#include "services/organization_service.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>
#include <QUrlQuery>

#include "models/organization_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "validator.hpp"

namespace pawspective::services {

OrganizationService::OrganizationService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void OrganizationService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
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

void OrganizationService::handleSuccess(
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

void OrganizationService::getOrganization(qint64 id) {
    m_networkClient.get(
        QUrl(QString("/orgs/%1").arg(id)),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::OrganizationDTO organization = models::OrganizationDTO::fromJson(obj);
                    emit getOrganizationSuccess(organization);
                },
                [this](QSharedPointer<BaseError> error) { emit getOrganizationFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getOrganizationFailed(error); });
        }
    );
}

void OrganizationService::createOrganization(const models::OrganizationRegisterDTO& dto) {
    const QJsonDocument doc(dto.toJson());

    utils::Validator validator;
    validator.field("name", dto.name.toStdString()).notBlank();
    if (auto error = validator.getValidationError()) {
        emit createOrganizationFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    m_networkClient.post(
        QUrl("/orgs"),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::OrganizationDTO organization = models::OrganizationDTO::fromJson(obj);
                    emit createOrganizationSuccess(organization);
                },
                [this](QSharedPointer<BaseError> error) { emit createOrganizationFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit createOrganizationFailed(error); });
        }
    );
}

void OrganizationService::handleSuccessArray(
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

void OrganizationService::findByNameContaining(const QString& name, int page) {
    utils::Validator validator;
    validator.field("name", name.toStdString()).notBlank();
    if (auto error = validator.getValidationError()) {
        emit findByNameContainingFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    QUrl url("/orgs");
    QUrlQuery query;
    query.addQueryItem("name", name);
    query.addQueryItem("page", QString::number(page));
    url.setQuery(query);

    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    emit findByNameContainingSuccess(models::OrganizationListDTO::fromJson(obj));
                },
                [this](QSharedPointer<BaseError> error) { emit findByNameContainingFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit findByNameContainingFailed(error); });
        }
    );
}

void OrganizationService::updateOrganization(qint64 id, const models::OrganizationUpdateDTO& dto) {
    const QJsonDocument doc(dto.toJson());
    utils::Validator validator;
    if (dto.name) {
        validator.field("name", dto.name->toStdString()).notBlank();
    }
    if (auto error = validator.getValidationError()) {
        emit updateOrganizationFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }
    m_networkClient.put(
        QUrl(QString("/orgs/%1").arg(id)),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::OrganizationDTO organization = models::OrganizationDTO::fromJson(obj);
                    emit updateOrganizationSuccess(organization);
                },
                [this](QSharedPointer<BaseError> error) { emit updateOrganizationFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit updateOrganizationFailed(error); });
        }
    );
}

}  // namespace pawspective::services
