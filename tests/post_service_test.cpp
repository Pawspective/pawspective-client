#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrlQuery>
#include <QtTest>

#include "models/post_create_dto.hpp"
#include "models/post_dto.hpp"
#include "models/post_update_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "services/post_service.hpp"

using namespace pawspective::models;   // NOLINT google-build-using-namespace
using namespace pawspective::services; // NOLINT google-build-using-namespace

Q_DECLARE_METATYPE(pawspective::models::PostDTO)
Q_DECLARE_METATYPE(pawspective::models::PostListDTO)

class FakeNetworkReply : public QNetworkReply {
    Q_OBJECT
public:
    explicit FakeNetworkReply(const QByteArray& data, QObject* parent = nullptr)
        : QNetworkReply(parent), m_data(data) {
        setProperty("responseData", data);
        open(QIODevice::ReadOnly);
    }
    void abort() override {}
    qint64 bytesAvailable() const override { return m_data.size() - m_pos; }
    bool isSequential() const override { return true; }

protected:
    qint64 readData(char* data, qint64 maxSize) override {
        qint64 n = qMin(maxSize, static_cast<qint64>(m_data.size() - m_pos));
        memcpy(data, m_data.constData() + m_pos, static_cast<size_t>(n));
        m_pos += n;
        return n;
    }

private:
    QByteArray m_data;
    qint64 m_pos = 0;
};

class MockNetworkClient : public INetworkClient {
public:
    using CallbackHandler = INetworkClient::CallbackHandler;

    struct Call {
        QUrl endpoint;
        QByteArray body;
        CallbackHandler onSuccess;
        CallbackHandler onError;
    };

    QList<Call> getCalls;
    QList<Call> postCalls;
    QList<Call> putCalls;
    QList<Call> deleteCalls;

    void get(const QUrl& url, CallbackHandler ok, CallbackHandler err) override {
        getCalls.append({url, {}, ok, err});
    }
    void post(const QUrl& url, const QByteArray& data, CallbackHandler ok, CallbackHandler err) override {
        postCalls.append({url, data, ok, err});
    }
    void put(const QUrl& url, const QByteArray& data, CallbackHandler ok, CallbackHandler err) override {
        putCalls.append({url, data, ok, err});
    }
    void patch(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void deleteResource(const QUrl& url, CallbackHandler ok, CallbackHandler err) override {
        deleteCalls.append({url, {}, ok, err});
    }

    void triggerSuccess(QList<Call>& calls, const QByteArray& data, int idx = 0) {
        auto *reply = new FakeNetworkReply(data);
        calls[idx].onSuccess(*reply);
        reply->deleteLater();
    }
    void triggerError(QList<Call>& calls, const QByteArray& data, int idx = 0) {
        auto *reply = new FakeNetworkReply(data);
        calls[idx].onError(*reply);
        reply->deleteLater();
    }
};

static QByteArray validPostJson(
    qint64 id = 1,
    qint64 organizationId = 5,
    const QString& text = "Test post content",
    const QString& createdAt = "2024-01-01T10:00:00Z"
) {
    QJsonObject post;
    post["id"] = id;
    post["organization_id"] = organizationId;
    post["text"] = text;
    post["created_at"] = createdAt;
    return QJsonDocument(post).toJson(QJsonDocument::Compact);
}

static QByteArray validPostListJson(int page = 1, int limit = 10, int totalCount = 1, int totalPages = 1) {
    QJsonArray items;
    items.append(QJsonDocument::fromJson(validPostJson()).object());

    QJsonObject root;
    root["items"] = items;
    root["page"] = page;
    root["limit"] = limit;
    root["total_count"] = totalCount;
    root["total_pages"] = totalPages;

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

class TestPostService : public QObject {
    Q_OBJECT

private slots:
    void testGetByOrganizationId_Success();
    void testGetByOrganizationId_NetworkError();
    void testGetByOrganizationId_InvalidJson();
    void testGetByOrganizationId_SendsOrgIdInQuery();

    void testCreatePost_Success();
    void testCreatePost_EmptyText();
    void testCreatePost_WhitespaceOnlyText();
    void testCreatePost_TextTooLong();
    void testCreatePost_NetworkError();
    void testCreatePost_InvalidJson();
    void testCreatePost_SendsPostToCorrectUrl();

    void testUpdatePost_Success();
    void testUpdatePost_EmptyText();
    void testUpdatePost_WhitespaceOnlyText();
    void testUpdatePost_TextTooLong();
    void testUpdatePost_EmptyText_DoesNotSendRequest();
    void testUpdatePost_NetworkError();
    void testUpdatePost_SendsPutToCorrectUrl();

    void testDeletePost_Success();
    void testDeletePost_NetworkError();
    void testDeletePost_SendsDeleteToCorrectUrl();
};

void TestPostService::testGetByOrganizationId_Success() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::getByOrganizationIdSuccess);
    QSignalSpy failedSpy(&service, &PostService::getByOrganizationIdFailed);

    service.getByOrganizationId(42);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validPostListJson(2, 10, 3, 1));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<PostListDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.page, 2);
    QCOMPARE(result.limit, 10);
    QCOMPARE(result.items.size(), 1);
    QCOMPARE(result.items[0].text, QString("Test post content"));
    QCOMPARE(result.items[0].organizationId, static_cast<qint64>(5));
}

void TestPostService::testGetByOrganizationId_NetworkError() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::getByOrganizationIdSuccess);
    QSignalSpy failedSpy(&service, &PostService::getByOrganizationIdFailed);

    service.getByOrganizationId(42);
    mock.triggerError(mock.getCalls, QByteArray("Internal Server Error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testGetByOrganizationId_InvalidJson() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::getByOrganizationIdSuccess);
    QSignalSpy failedSpy(&service, &PostService::getByOrganizationIdFailed);

    service.getByOrganizationId(42);
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testGetByOrganizationId_SendsOrgIdInQuery() {
    MockNetworkClient mock;
    PostService service(mock);

    service.getByOrganizationId(77);

    QCOMPARE(mock.getCalls.size(), 1);
    QUrl url = mock.getCalls[0].endpoint;
    QUrlQuery query(url);
    QCOMPARE(url.path(), QString("/posts"));
    QCOMPARE(query.queryItemValue("org_id"), QString("77"));
    QVERIFY(query.queryItemValue("page").isEmpty());
    QVERIFY(query.queryItemValue("limit").isEmpty());
}

void TestPostService::testCreatePost_Success() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::createPostSuccess);
    QSignalSpy failedSpy(&service, &PostService::createPostFailed);

    PostCreateDTO dto;
    dto.text = "New post content";

    service.createPost(dto);
    QCOMPARE(mock.postCalls.size(), 1);

    QCOMPARE(mock.postCalls[0].endpoint.path(), QString("/posts"));
    QJsonObject body = QJsonDocument::fromJson(mock.postCalls[0].body).object();
    QCOMPARE(body["text"].toString(), QString("New post content"));

    mock.triggerSuccess(mock.postCalls, validPostJson(3, 5, "New post content"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<PostDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.id, static_cast<qint64>(3));
    QCOMPARE(result.text, QString("New post content"));
    QCOMPARE(result.organizationId, static_cast<qint64>(5));
}

void TestPostService::testCreatePost_EmptyText() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::createPostSuccess);
    QSignalSpy failedSpy(&service, &PostService::createPostFailed);

    PostCreateDTO dto;
    dto.text = "";

    service.createPost(dto);

    QCOMPARE(mock.postCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);

    auto error = qvariant_cast<QSharedPointer<BaseError>>(failedSpy.at(0).at(0)).dynamicCast<ValidationError>();
    QVERIFY(!error.isNull());
    QVERIFY(!error->getErrors().empty());
    QCOMPARE(QString::fromStdString(error->getErrors()[0].fieldName), QString("text"));
}

void TestPostService::testCreatePost_WhitespaceOnlyText() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::createPostSuccess);
    QSignalSpy failedSpy(&service, &PostService::createPostFailed);

    PostCreateDTO dto;
    dto.text = "   ";

    service.createPost(dto);

    QCOMPARE(mock.postCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);

    auto error = qvariant_cast<QSharedPointer<BaseError>>(failedSpy.at(0).at(0)).dynamicCast<ValidationError>();
    QVERIFY(!error.isNull());
    QCOMPARE(QString::fromStdString(error->getErrors()[0].fieldName), QString("text"));
}

void TestPostService::testCreatePost_TextTooLong() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::createPostSuccess);
    QSignalSpy failedSpy(&service, &PostService::createPostFailed);

    PostCreateDTO dto;
    dto.text = QString(2001, 'a'); 

    service.createPost(dto);

    QCOMPARE(mock.postCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testCreatePost_NetworkError() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::createPostSuccess);
    QSignalSpy failedSpy(&service, &PostService::createPostFailed);

    PostCreateDTO dto;
    dto.text = "Some text";

    service.createPost(dto);
    mock.triggerError(mock.postCalls, QByteArray("Server Error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testCreatePost_InvalidJson() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::createPostSuccess);
    QSignalSpy failedSpy(&service, &PostService::createPostFailed);

    PostCreateDTO dto;
    dto.text = "Some text";

    service.createPost(dto);
    mock.triggerSuccess(mock.postCalls, QByteArray("not valid json"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testCreatePost_SendsPostToCorrectUrl() {
    MockNetworkClient mock;
    PostService service(mock);

    PostCreateDTO dto;
    dto.text = "Some text";

    service.createPost(dto);
    QCOMPARE(mock.postCalls.size(), 1);
    QCOMPARE(mock.postCalls[0].endpoint.path(), QString("/posts"));
}

void TestPostService::testUpdatePost_Success() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::updatePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::updatePostFailed);

    PostUpdateDTO dto;
    dto.text = "Updated text";

    service.updatePost(3, dto);
    QCOMPARE(mock.putCalls.size(), 1);

    QCOMPARE(mock.putCalls[0].endpoint.path(), QString("/posts/3"));
    QJsonObject body = QJsonDocument::fromJson(mock.putCalls[0].body).object();
    QCOMPARE(body["text"].toString(), QString("Updated text"));

    mock.triggerSuccess(mock.putCalls, validPostJson(3, 5, "Updated text"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<PostDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.id, static_cast<qint64>(3));
    QCOMPARE(result.text, QString("Updated text"));
}

void TestPostService::testUpdatePost_EmptyText() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::updatePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::updatePostFailed);

    PostUpdateDTO dto;
    dto.text = "";

    service.updatePost(3, dto);

    QCOMPARE(mock.putCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testUpdatePost_WhitespaceOnlyText() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::updatePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::updatePostFailed);

    PostUpdateDTO dto;
    dto.text = "   ";

    service.updatePost(3, dto);

    QCOMPARE(mock.putCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testUpdatePost_TextTooLong() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::updatePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::updatePostFailed);

    PostUpdateDTO dto;
    dto.text = QString(2001, 'a');

    service.updatePost(3, dto);

    QCOMPARE(mock.putCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testUpdatePost_EmptyText_DoesNotSendRequest() {
    MockNetworkClient mock;
    PostService service(mock);

    PostUpdateDTO dto;
    dto.text = "";

    service.updatePost(3, dto);
    QCOMPARE(mock.putCalls.size(), 0);
}

void TestPostService::testUpdatePost_NetworkError() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::updatePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::updatePostFailed);

    PostUpdateDTO dto;
    dto.text = "Some text";

    service.updatePost(3, dto);
    mock.triggerError(mock.putCalls, QByteArray("Server Error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testUpdatePost_SendsPutToCorrectUrl() {
    MockNetworkClient mock;
    PostService service(mock);

    PostUpdateDTO dto;
    dto.text = "Some text";

    service.updatePost(42, dto);
    QCOMPARE(mock.putCalls.size(), 1);
    QCOMPARE(mock.putCalls[0].endpoint.path(), QString("/posts/42"));
}

void TestPostService::testDeletePost_Success() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::deletePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::deletePostFailed);

    service.deletePost(5);
    QCOMPARE(mock.deleteCalls.size(), 1);

    mock.triggerSuccess(mock.deleteCalls, {});

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
}

void TestPostService::testDeletePost_NetworkError() {
    MockNetworkClient mock;
    PostService service(mock);

    QSignalSpy successSpy(&service, &PostService::deletePostSuccess);
    QSignalSpy failedSpy(&service, &PostService::deletePostFailed);

    service.deletePost(5);
    mock.triggerError(mock.deleteCalls, QByteArray("Not Found"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestPostService::testDeletePost_SendsDeleteToCorrectUrl() {
    MockNetworkClient mock;
    PostService service(mock);

    service.deletePost(77);
    QCOMPARE(mock.deleteCalls.size(), 1);
    QCOMPARE(mock.deleteCalls[0].endpoint.path(), QString("/posts/77"));
}

QTEST_MAIN(TestPostService)

#include "post_service_test.moc"