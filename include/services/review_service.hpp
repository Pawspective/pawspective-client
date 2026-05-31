#pragma once

#include <QObject>

#include "i_network_client.hpp"
#include "models/review_create_dto.hpp"
#include "models/review_dto.hpp"
#include "models/review_update_dto.hpp"
#include "services/errors.hpp"

namespace pawspective::services {
class ReviewService : public QObject {
    Q_OBJECT
public:
    explicit ReviewService(INetworkClient& networkClient, QObject* parent = nullptr);

    void getByOrganizationId(qint64 id, int page = 1, int limit = 10);
    void createReview(const models::ReviewCreateDTO& dto);
    void updateReview(qint64 id, const models::ReviewUpdateDTO& dto);
    void deleteReview(qint64 id);

signals:
    void getByOrganizationIdSuccess(const models::ReviewListDTO& reviews);
    void createReviewSuccess(const models::ReviewDTO& review);
    void updateReviewSuccess(const models::ReviewDTO& review);
    void deleteReviewSuccess();

    void getByOrganizationIdFailed(QSharedPointer<BaseError> error);
    void createReviewFailed(QSharedPointer<BaseError> error);
    void updateReviewFailed(QSharedPointer<BaseError> error);
    void deleteReviewFailed(QSharedPointer<BaseError> error);

private:
    void handleError(QNetworkReply& reply, std::function<void(QSharedPointer<BaseError>)> onError);
    void handleSuccess(
        QNetworkReply& reply,
        std::function<void(const QJsonObject&)> onSuccess,
        std::function<void(QSharedPointer<BaseError>)> onError
    );

    INetworkClient& m_networkClient;
};
}  // namespace pawspective::services
