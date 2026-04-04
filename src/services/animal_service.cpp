#include "services/animal_service.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrl>
#include <QUrlQuery>

#include "models/animal_dto.hpp"
#include "models/animal_enums.hpp"
#include "models/animal_filter_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "validator.hpp"

namespace pawspective::services {

AnimalService::AnimalService(INetworkClient& networkClient, QObject* parent)
    : QObject(parent), m_networkClient(networkClient) {}

void AnimalService::handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

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

void AnimalService::handleSuccess(
    QNetworkReply& reply,
    std::function<void(const QJsonObject&)> onSuccess,
    std::function<void(QSharedPointer<BaseError>)> onError
) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
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

void AnimalService::handleSuccessArray(
    QNetworkReply& reply,
    std::function<void(const QJsonArray&)> onSuccess,
    std::function<void(QSharedPointer<BaseError>)> onError
) {
    QJsonParseError parseError;
    QByteArray data = reply.readAll();
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

void AnimalService::getAnimals(const models::AnimalFilterDTO& filter) {
    QUrl url("/animals");
    QUrlQuery query;

    if (filter.breeds) {
        for (int64_t breedId : *filter.breeds) {
            query.addQueryItem("breedId", QString::number(breedId));
        }
    }
    if (filter.animalTypes) {
        for (models::AnimalType type : *filter.animalTypes) {
            query.addQueryItem("animalType", models::toApiString(type));
        }
    }
    if (filter.sizes) {
        for (models::AnimalSize size : *filter.sizes) {
            query.addQueryItem("size", models::toApiString(size));
        }
    }
    if (filter.genders) {
        for (models::AnimalGender gender : *filter.genders) {
            query.addQueryItem("gender", models::toApiString(gender));
        }
    }
    if (filter.careLevels) {
        for (models::CareLevel level : *filter.careLevels) {
            query.addQueryItem("careLevel", models::toApiString(level));
        }
    }
    if (filter.colors) {
        for (models::AnimalColor color : *filter.colors) {
            query.addQueryItem("color", models::toApiString(color));
        }
    }
    if (filter.goodWiths) {
        for (models::GoodWith goodWith : *filter.goodWiths) {
            query.addQueryItem("goodWith", models::toApiString(goodWith));
        }
    }
    if (filter.ageLte) {
        query.addQueryItem("ageLte", QString::number(*filter.ageLte));
    }
    if (filter.ageGte) {
        query.addQueryItem("ageGte", QString::number(*filter.ageGte));
    }

    if (!query.isEmpty()) {
        url.setQuery(query);
    }

    m_networkClient.get(
        url,
        [this](QNetworkReply& reply) {
            handleSuccessArray(
                reply,
                [this](const QJsonArray& arr) {
                    QList<models::AnimalDTO> animals;
                    for (const auto& item : arr) {
                        animals.append(models::AnimalDTO::fromJson(item.toObject()));
                    }
                    emit getAnimalsSuccess(animals);
                },
                [this](QSharedPointer<BaseError> error) { emit getAnimalsFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getAnimalsFailed(error); });
        }
    );
}

void AnimalService::getAnimal(qint64 id) {
    utils::Validator validator;
    validator.field("id", std::to_string(id)).matches(QRegularExpression("[1-9][0-9]*"), "must be a positive number");
    if (auto error = validator.getValidationError()) {
        emit getAnimalFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    m_networkClient.get(
        QUrl(QString("/animals/%1").arg(id)),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::AnimalDTO animal = models::AnimalDTO::fromJson(obj);
                    emit getAnimalSuccess(animal);
                },
                [this](QSharedPointer<BaseError> error) { emit getAnimalFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getAnimalFailed(error); });
        }
    );
}

void AnimalService::createAnimal(const models::AnimalRegisterDTO& dto) {
    utils::Validator validator;
    validator.field("name", dto.name.toStdString()).notBlank().maxLength(255);
    validator.field("age", std::to_string(dto.age)).matches(QRegularExpression("\\d+"), "must be non-negative");
    if (auto error = validator.getValidationError()) {
        emit createAnimalFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    const QJsonDocument doc(dto.toJson());

    m_networkClient.post(
        QUrl("/animals"),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::AnimalDTO animal = models::AnimalDTO::fromJson(obj);
                    emit createAnimalSuccess(animal);
                },
                [this](QSharedPointer<BaseError> error) { emit createAnimalFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit createAnimalFailed(error); });
        }
    );
}

void AnimalService::updateAnimal(qint64 id, const models::AnimalUpdateDTO& dto) {
    utils::Validator validator;
    validator.field("id", std::to_string(id)).matches(QRegularExpression("[1-9][0-9]*"), "must be a positive number");
    if (dto.name) {
        validator.field("name", dto.name->toStdString()).notBlank().maxLength(255);
    }
    if (dto.age) {
        validator.field("age", std::to_string(*dto.age)).matches(QRegularExpression("\\d+"), "must be non-negative");
    }
    if (auto error = validator.getValidationError()) {
        emit updateAnimalFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    const QJsonDocument doc(dto.toJson());

    m_networkClient.put(
        QUrl(QString("/animals/%1").arg(id)),
        doc.toJson(QJsonDocument::Compact),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::AnimalDTO animal = models::AnimalDTO::fromJson(obj);
                    emit updateAnimalSuccess(animal);
                },
                [this](QSharedPointer<BaseError> error) { emit updateAnimalFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit updateAnimalFailed(error); });
        }
    );
}

void AnimalService::getAnimalFilters() {
    m_networkClient.get(
        QUrl("/animals/filters"),
        [this](QNetworkReply& reply) {
            handleSuccess(
                reply,
                [this](const QJsonObject& obj) {
                    models::AnimalFilterDTO filters = models::AnimalFilterDTO::fromJson(obj);
                    emit getAnimalFiltersSuccess(filters);
                },
                [this](QSharedPointer<BaseError> error) { emit getAnimalFiltersFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getAnimalFiltersFailed(error); });
        }
    );
}

void AnimalService::getAnimalsByOrganization(qint64 organizationId) {
    utils::Validator validator;
    validator.field("organization_id", std::to_string(organizationId))
        .matches(QRegularExpression("[1-9][0-9]*"), "must be a positive number");
    if (auto error = validator.getValidationError()) {
        emit getAnimalsByOrganizationFailed(QSharedPointer<BaseError>(new ValidationError(std::move(*error))));
        return;
    }

    m_networkClient.get(
        QUrl(QString("/org/%1/animals").arg(organizationId)),
        [this](QNetworkReply& reply) {
            handleSuccessArray(
                reply,
                [this](const QJsonArray& arr) {
                    QList<models::AnimalDTO> animals;
                    for (const auto& item : arr) {
                        animals.append(models::AnimalDTO::fromJson(item.toObject()));
                    }
                    emit getAnimalsByOrganizationSuccess(animals);
                },
                [this](QSharedPointer<BaseError> error) { emit getAnimalsByOrganizationFailed(error); }
            );
        },
        [this](QNetworkReply& reply) {
            handleError(reply, [this](QSharedPointer<BaseError> error) { emit getAnimalsByOrganizationFailed(error); });
        }
    );
}

}  // namespace pawspective::services
