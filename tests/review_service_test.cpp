#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QUrlQuery>
#include <QtTest>

#include "models/review_create_dto.hpp"
#include "models/review_dto.hpp"
#include "models/review_update_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "services/review_service.hpp"

using namespace pawspective::models;   // NOLINT google-build-using-namespace
using namespace pawspective::services; // NOLINT google-build-using-namespace

Q_DECLARE_METATYPE(pawspective::models::ReviewDTO)
Q_DECLARE_METATYPE(pawspective::models::ReviewListDTO)

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

static QByteArray validReviewJson(
    qint64 id = 1,
    qint64 animalId = 10,
    const QString& animalName = "Buddy",
    const QString& text = "Great care",
    const QString& createdAt = "2024-01-01T10:00:00Z",
    bool canEdit = true
) {
    QJsonObject review;
    review["id"] = id;
    review["animal_id"] = animalId;
    review["animal_name"] = animalName;
    review["text"] = text;
    review["created_at"] = createdAt;
    review["can_edit"] = canEdit;
    return QJsonDocument(review).toJson(QJsonDocument::Compact);
}

static QByteArray validReviewListJson(int page = 1, int limit = 10, int totalCount = 1, int totalPages = 1) {
    QJsonArray items;
    items.append(QJsonDocument::fromJson(validReviewJson()).object());

    QJsonObject root;
    root["items"] = items;
    root["page"] = page;
    root["limit"] = limit;
    root["total_count"] = totalCount;
    root["total_pages"] = totalPages;

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}


class TestReviewService : public QObject {
    Q_OBJECT

private slots:
    void testGetByOrganizationId_Success();
    void testGetByOrganizationId_SendsOrgIdInQuery();

    void testCreateReview_Success();
    void testCreateReview_EmptyText();

    void testUpdateReview_Success();
};


void TestReviewService::testGetByOrganizationId_Success() {
    MockNetworkClient mock;
    ReviewService service(mock);

    QSignalSpy successSpy(&service, &ReviewService::getByOrganizationIdSuccess);
    QSignalSpy failedSpy(&service, &ReviewService::getByOrganizationIdFailed);

    service.getByOrganizationId(42);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validReviewListJson(2, 10, 3, 1));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<ReviewListDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.page, 2);
    QCOMPARE(result.limit, 10);
    QCOMPARE(result.items.size(), 1);
    QCOMPARE(result.items[0].animalName, QString("Buddy"));
}

void TestReviewService::testGetByOrganizationId_SendsOrgIdInQuery() {
    MockNetworkClient mock;
    ReviewService service(mock);

    service.getByOrganizationId(77);

    QCOMPARE(mock.getCalls.size(), 1);
    QUrl url = mock.getCalls[0].endpoint;
    QUrlQuery query(url);
    QCOMPARE(url.path(), QString("/reviews"));
    QCOMPARE(query.queryItemValue("org_id"), QString("77"));
}

void TestReviewService::testCreateReview_Success() {
    MockNetworkClient mock;
    ReviewService service(mock);

    QSignalSpy successSpy(&service, &ReviewService::createReviewSuccess);
    QSignalSpy failedSpy(&service, &ReviewService::createReviewFailed);

    ReviewCreateDTO dto;
    dto.animalId = 10;
    dto.text = "Great care";

    service.createReview(dto);
    QCOMPARE(mock.postCalls.size(), 1);

    QCOMPARE(mock.postCalls[0].endpoint.path(), QString("/reviews"));
    QJsonObject body = QJsonDocument::fromJson(mock.postCalls[0].body).object();
    QCOMPARE(body["animal_id"].toVariant().toLongLong(), static_cast<qint64>(10));
    QCOMPARE(body["text"].toString(), QString("Great care"));

    mock.triggerSuccess(mock.postCalls, validReviewJson(3, 10, "Buddy", "Great care"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<ReviewDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.id, static_cast<qint64>(3));
    QCOMPARE(result.animalId, static_cast<qint64>(10));
    QCOMPARE(result.text, QString("Great care"));
    QCOMPARE(result.canEdit, true);
}

void TestReviewService::testCreateReview_EmptyText() {
    MockNetworkClient mock;
    ReviewService service(mock);

    QSignalSpy successSpy(&service, &ReviewService::createReviewSuccess);
    QSignalSpy failedSpy(&service, &ReviewService::createReviewFailed);

    ReviewCreateDTO dto;
    dto.animalId = 10;
    dto.text = "   ";

    service.createReview(dto);

    QCOMPARE(mock.postCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);

    auto error = qvariant_cast<QSharedPointer<BaseError>>(failedSpy.at(0).at(0)).dynamicCast<ValidationError>();
    QVERIFY(!error.isNull());
    QVERIFY(!error->getErrors().empty());
    QCOMPARE(QString::fromStdString(error->getErrors()[0].fieldName), QString("text"));
}

void TestReviewService::testUpdateReview_Success() {
    MockNetworkClient mock;
    ReviewService service(mock);

    QSignalSpy successSpy(&service, &ReviewService::updateReviewSuccess);
    QSignalSpy failedSpy(&service, &ReviewService::updateReviewFailed);

    ReviewUpdateDTO dto;
    dto.text = "Updated text";

    service.updateReview(3, dto);
    QCOMPARE(mock.putCalls.size(), 1);

    QCOMPARE(mock.putCalls[0].endpoint.path(), QString("/reviews/3"));
    QJsonObject body = QJsonDocument::fromJson(mock.putCalls[0].body).object();
    QCOMPARE(body["text"].toString(), QString("Updated text"));

    mock.triggerSuccess(mock.putCalls, validReviewJson(3, 10, "Buddy", "Updated text"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<ReviewDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.id, static_cast<qint64>(3));
    QCOMPARE(result.text, QString("Updated text"));
}


QTEST_MAIN(TestReviewService)

#include "review_service_test.moc"
