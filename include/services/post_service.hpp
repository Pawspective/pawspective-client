#pragma once

#include <QObject>

#include "i_network_client.hpp"
#include "models/post_create_dto.hpp"
#include "models/post_dto.hpp"
#include "models/post_update_dto.hpp"
#include "services/errors.hpp"

namespace pawspective::services {
class PostService : public QObject {
    Q_OBJECT
public:
    explicit PostService(INetworkClient& networkClient, QObject* parent = nullptr);

    void getByOrganizationId(qint64 id);
    void createPost(const models::PostCreateDTO& dto);
    void updatePost(qint64 id, const models::PostUpdateDTO& dto);
    void deletePost(qint64 id);

signals:
    void getByOrganizationIdSuccess(const models::PostListDTO& posts);
    void createPostSuccess(const models::PostDTO& post);
    void updatePostSuccess(const models::PostDTO& post);
    void deletePostSuccess();

    void getByOrganizationIdFailed(QSharedPointer<BaseError> error);
    void createPostFailed(QSharedPointer<BaseError> error);
    void updatePostFailed(QSharedPointer<BaseError> error);
    void deletePostFailed(QSharedPointer<BaseError> error);

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
