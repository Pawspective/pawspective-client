#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QtTest>

#include "models/adopt_request_dto.hpp"
#include "services/adopt_request_service.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

using namespace pawspective::models;   // NOLINT google-build-using-namespace
using namespace pawspective::services; // NOLINT google-build-using-namespace

// ---------------------------------------------------------------------------
// Fake QNetworkReply that serves pre-baked data

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

// ---------------------------------------------------------------------------
// MockNetworkClient — captures callbacks; lets tests trigger them manually

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

    void get(const QUrl& url, CallbackHandler ok, CallbackHandler err) override {
        getCalls.append({url, {}, ok, err});
    }
    void post(const QUrl& url, const QByteArray& data, CallbackHandler ok, CallbackHandler err) override {
        postCalls.append({url, data, ok, err});
    }
    void put(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void patch(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void deleteResource(const QUrl&, CallbackHandler, CallbackHandler) override {}
    void postRaw(const QUrl&, const QByteArray&, const QString&, CallbackHandler, CallbackHandler) override {}

    void triggerSuccess(QList<Call>& calls, const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        calls[idx].onSuccess(reply);
    }
    void triggerError(QList<Call>& calls, const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        calls[idx].onError(reply);
    }
};

// ---------------------------------------------------------------------------
// Helpers

static QJsonObject animalJson(qint64 id = 10, const QString& name = "Buddy") {
    QJsonObject breed;
    breed["id"] = 1;
    breed["animal_type"] = "dog";
    breed["name"] = "Labrador";

    QJsonObject animal;
    animal["id"] = id;
    animal["organization_id"] = 5;
    animal["name"] = name;
    animal["breed"] = breed;
    animal["size"] = "medium";
    animal["gender"] = "male";
    animal["care_level"] = "easy";
    animal["color"] = "black";
    animal["good_with"] = "dogs";
    animal["age"] = 3;
    animal["status"] = "available";
    animal["can_be_adopted"] = true;
    return animal;
}

static QByteArray validAdoptRequestJson(qint64 id = 1, const QString& email = "user@example.com") {
    QJsonObject req;
    req["id"] = id;
    req["email"] = email;
    req["animal"] = animalJson();
    return QJsonDocument(req).toJson(QJsonDocument::Compact);
}

static QByteArray serverErrorJson(const QString& message = "Not found") {
    QJsonObject err;
    err["message"] = message;
    return QJsonDocument(err).toJson(QJsonDocument::Compact);
}

static QByteArray validAdoptRequestListJson(int page = 1, int limit = 10, int totalCount = 1, int totalPages = 1) {
    QJsonArray items;
    items.append(QJsonDocument::fromJson(validAdoptRequestJson()).object());

    QJsonObject root;
    root["items"] = items;
    root["page"] = page;
    root["limit"] = limit;
    root["total_count"] = totalCount;
    root["total_pages"] = totalPages;
    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

// ---------------------------------------------------------------------------

class TestAdoptRequestService : public QObject {
    Q_OBJECT

private slots:
    // DTO tests
    void testAdoptRequestDtoFromJson_ValidObject();
    void testAdoptRequestDtoFromJson_MissingId_Throws();
    void testAdoptRequestDtoFromJson_MissingEmail_Throws();
    void testAdoptRequestDtoToJson_RoundTrip();
    void testAdoptRequestListDtoFromJson_ValidObject();

    // createAdoptRequest tests
    void testCreateAdoptRequest_Success_EmitsCreateAdoptRequestSuccess();
    void testCreateAdoptRequest_NetworkError_EmitsCreateAdoptRequestFailed();
    void testCreateAdoptRequest_InvalidJson_EmitsCreateAdoptRequestFailed();
    void testCreateAdoptRequest_SendsPostToCorrectUrl();
    void testCreateAdoptRequest_ServerError_DoesNotEmitOtherFailedSignals();

    // getAdoptRequests tests
    void testGetAdoptRequests_Success_EmitsGetAdoptRequestsSuccess();
    void testGetAdoptRequests_Success_PaginationFieldsAreCorrect();
    void testGetAdoptRequests_NetworkError_EmitsGetAdoptRequestsFailed();
    void testGetAdoptRequests_InvalidJson_EmitsGetAdoptRequestsFailed();
    void testGetAdoptRequests_SendsOrgIdAndPageInQuery();
    void testGetAdoptRequests_DefaultPage_SendsPageOne();
    void testGetAdoptRequests_ServerError_DoesNotEmitOtherFailedSignals();

    // acceptAdoptRequest tests
    void testAcceptAdoptRequest_Success_EmitsAcceptAdoptRequestSuccess();
    void testAcceptAdoptRequest_NetworkError_EmitsAcceptAdoptRequestFailed();
    void testAcceptAdoptRequest_SendsPostToCorrectUrl();
    void testAcceptAdoptRequest_ServerError_DoesNotEmitOtherFailedSignals();

    // denyAdoptRequest tests
    void testDenyAdoptRequest_Success_EmitsDenyAdoptRequestSuccess();
    void testDenyAdoptRequest_NetworkError_EmitsDenyAdoptRequestFailed();
    void testDenyAdoptRequest_SendsPostToCorrectUrl();
    void testDenyAdoptRequest_ServerError_DoesNotEmitOtherFailedSignals();
};

// ---------------------------------------------------------------------------
// DTO tests

void TestAdoptRequestService::testAdoptRequestDtoFromJson_ValidObject() {
    QJsonObject json = QJsonDocument::fromJson(validAdoptRequestJson(42, "adopter@example.com")).object();

    AdoptRequestDTO dto = AdoptRequestDTO::fromJson(json);

    QCOMPARE(dto.id, static_cast<qint64>(42));
    QCOMPARE(dto.email, QString("adopter@example.com"));
    QCOMPARE(dto.animal.id, static_cast<qint64>(10));
    QCOMPARE(dto.animal.name, QString("Buddy"));
}

void TestAdoptRequestService::testAdoptRequestDtoFromJson_MissingId_Throws() {
    QJsonObject json;
    json["email"] = "user@example.com";
    json["animal"] = animalJson();

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, AdoptRequestDTO::fromJson(json));
}

void TestAdoptRequestService::testAdoptRequestDtoFromJson_MissingEmail_Throws() {
    QJsonObject json;
    json["id"] = 1;
    json["animal"] = animalJson();

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, AdoptRequestDTO::fromJson(json));
}

void TestAdoptRequestService::testAdoptRequestDtoToJson_RoundTrip() {
    QJsonObject json = QJsonDocument::fromJson(validAdoptRequestJson(7, "round@trip.com")).object();
    AdoptRequestDTO dto = AdoptRequestDTO::fromJson(json);
    QJsonObject result = dto.toJson();

    QCOMPARE(result["id"].toVariant().toLongLong(), static_cast<qint64>(7));
    QCOMPARE(result["email"].toString(), QString("round@trip.com"));
}

void TestAdoptRequestService::testAdoptRequestListDtoFromJson_ValidObject() {
    QByteArray data = validAdoptRequestListJson(2, 10, 25, 3);
    AdoptRequestListDTO dto = AdoptRequestListDTO::fromJson(QJsonDocument::fromJson(data).object());

    QCOMPARE(dto.page, 2);
    QCOMPARE(dto.limit, 10);
    QCOMPARE(dto.totalCount, static_cast<qint64>(25));
    QCOMPARE(dto.totalPages, static_cast<qint64>(3));
    QCOMPARE(dto.items.size(), 1);
    QCOMPARE(dto.items.at(0).id, static_cast<qint64>(1));
    QCOMPARE(dto.items.at(0).email, QString("user@example.com"));
}

// ---------------------------------------------------------------------------
// createAdoptRequest tests

void TestAdoptRequestService::testCreateAdoptRequest_Success_EmitsCreateAdoptRequestSuccess() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::createAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::createAdoptRequestFailed);

    service.createAdoptRequest(10);
    QCOMPARE(mock.postCalls.size(), 1);

    mock.triggerSuccess(mock.postCalls, validAdoptRequestJson(1, "user@example.com"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto result = qvariant_cast<AdoptRequestDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.id, static_cast<qint64>(1));
    QCOMPARE(result.email, QString("user@example.com"));
}

void TestAdoptRequestService::testCreateAdoptRequest_NetworkError_EmitsCreateAdoptRequestFailed() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::createAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::createAdoptRequestFailed);

    service.createAdoptRequest(10);
    mock.triggerError(mock.postCalls, serverErrorJson("Conflict"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAdoptRequestService::testCreateAdoptRequest_InvalidJson_EmitsCreateAdoptRequestFailed() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::createAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::createAdoptRequestFailed);

    service.createAdoptRequest(10);
    mock.triggerSuccess(mock.postCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAdoptRequestService::testCreateAdoptRequest_SendsPostToCorrectUrl() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    service.createAdoptRequest(42);
    QCOMPARE(mock.postCalls.size(), 1);
    QCOMPARE(mock.postCalls.at(0).endpoint.path(), QString("/animals/42/adopt"));
}

void TestAdoptRequestService::testCreateAdoptRequest_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy getAdoptRequestsFailed(&service, &AdoptRequestService::getAdoptRequestsFailed);
    QSignalSpy acceptFailed(&service, &AdoptRequestService::acceptAdoptRequestFailed);
    QSignalSpy createFailed(&service, &AdoptRequestService::createAdoptRequestFailed);

    service.createAdoptRequest(10);
    mock.triggerError(mock.postCalls, serverErrorJson());

    QCOMPARE(createFailed.count(), 1);
    QCOMPARE(getAdoptRequestsFailed.count(), 0);
    QCOMPARE(acceptFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// getAdoptRequests tests

void TestAdoptRequestService::testGetAdoptRequests_Success_EmitsGetAdoptRequestsSuccess() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::getAdoptRequestsSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::getAdoptRequestsFailed);

    service.getAdoptRequests(5);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validAdoptRequestListJson());

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
}

void TestAdoptRequestService::testGetAdoptRequests_Success_PaginationFieldsAreCorrect() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::getAdoptRequestsSuccess);

    service.getAdoptRequests(5, 2);
    mock.triggerSuccess(mock.getCalls, validAdoptRequestListJson(2, 10, 25, 3));

    QCOMPARE(successSpy.count(), 1);
    auto result = qvariant_cast<AdoptRequestListDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.page, 2);
    QCOMPARE(result.limit, 10);
    QCOMPARE(result.totalCount, static_cast<qint64>(25));
    QCOMPARE(result.totalPages, static_cast<qint64>(3));
    QCOMPARE(result.items.size(), 1);
}

void TestAdoptRequestService::testGetAdoptRequests_NetworkError_EmitsGetAdoptRequestsFailed() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::getAdoptRequestsSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::getAdoptRequestsFailed);

    service.getAdoptRequests(5);
    mock.triggerError(mock.getCalls, serverErrorJson("Forbidden"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAdoptRequestService::testGetAdoptRequests_InvalidJson_EmitsGetAdoptRequestsFailed() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::getAdoptRequestsSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::getAdoptRequestsFailed);

    service.getAdoptRequests(5);
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAdoptRequestService::testGetAdoptRequests_SendsOrgIdAndPageInQuery() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    service.getAdoptRequests(7, 3);
    QCOMPARE(mock.getCalls.size(), 1);

    const QUrlQuery query(mock.getCalls.at(0).endpoint.query());
    QCOMPARE(query.queryItemValue("org_id"), QString("7"));
    QCOMPARE(query.queryItemValue("page"), QString("3"));
}

void TestAdoptRequestService::testGetAdoptRequests_DefaultPage_SendsPageOne() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    service.getAdoptRequests(5);
    QCOMPARE(mock.getCalls.size(), 1);

    const QUrlQuery query(mock.getCalls.at(0).endpoint.query());
    QCOMPARE(query.queryItemValue("page"), QString("1"));
}

void TestAdoptRequestService::testGetAdoptRequests_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy createFailed(&service, &AdoptRequestService::createAdoptRequestFailed);
    QSignalSpy acceptFailed(&service, &AdoptRequestService::acceptAdoptRequestFailed);
    QSignalSpy getFailed(&service, &AdoptRequestService::getAdoptRequestsFailed);

    service.getAdoptRequests(5);
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(getFailed.count(), 1);
    QCOMPARE(createFailed.count(), 0);
    QCOMPARE(acceptFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// acceptAdoptRequest tests

void TestAdoptRequestService::testAcceptAdoptRequest_Success_EmitsAcceptAdoptRequestSuccess() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::acceptAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::acceptAdoptRequestFailed);

    service.acceptAdoptRequest(3);
    QCOMPARE(mock.postCalls.size(), 1);

    mock.triggerSuccess(mock.postCalls, {});

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
}

void TestAdoptRequestService::testAcceptAdoptRequest_NetworkError_EmitsAcceptAdoptRequestFailed() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::acceptAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::acceptAdoptRequestFailed);

    service.acceptAdoptRequest(3);
    mock.triggerError(mock.postCalls, serverErrorJson("Forbidden"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAdoptRequestService::testAcceptAdoptRequest_SendsPostToCorrectUrl() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    service.acceptAdoptRequest(99);
    QCOMPARE(mock.postCalls.size(), 1);
    QCOMPARE(mock.postCalls.at(0).endpoint.path(), QString("/requests/accept/99"));
}

void TestAdoptRequestService::testAcceptAdoptRequest_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy createFailed(&service, &AdoptRequestService::createAdoptRequestFailed);
    QSignalSpy denyFailed(&service, &AdoptRequestService::denyAdoptRequestFailed);
    QSignalSpy acceptFailed(&service, &AdoptRequestService::acceptAdoptRequestFailed);

    service.acceptAdoptRequest(3);
    mock.triggerError(mock.postCalls, serverErrorJson());

    QCOMPARE(acceptFailed.count(), 1);
    QCOMPARE(createFailed.count(), 0);
    QCOMPARE(denyFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// denyAdoptRequest tests

void TestAdoptRequestService::testDenyAdoptRequest_Success_EmitsDenyAdoptRequestSuccess() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::denyAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::denyAdoptRequestFailed);

    service.denyAdoptRequest(4);
    QCOMPARE(mock.postCalls.size(), 1);

    mock.triggerSuccess(mock.postCalls, {});

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
}

void TestAdoptRequestService::testDenyAdoptRequest_NetworkError_EmitsDenyAdoptRequestFailed() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy successSpy(&service, &AdoptRequestService::denyAdoptRequestSuccess);
    QSignalSpy failedSpy(&service, &AdoptRequestService::denyAdoptRequestFailed);

    service.denyAdoptRequest(4);
    mock.triggerError(mock.postCalls, serverErrorJson("Forbidden"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAdoptRequestService::testDenyAdoptRequest_SendsPostToCorrectUrl() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    service.denyAdoptRequest(77);
    QCOMPARE(mock.postCalls.size(), 1);
    QCOMPARE(mock.postCalls.at(0).endpoint.path(), QString("/requests/deny/77"));
}

void TestAdoptRequestService::testDenyAdoptRequest_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    AdoptRequestService service(mock);

    QSignalSpy createFailed(&service, &AdoptRequestService::createAdoptRequestFailed);
    QSignalSpy acceptFailed(&service, &AdoptRequestService::acceptAdoptRequestFailed);
    QSignalSpy denyFailed(&service, &AdoptRequestService::denyAdoptRequestFailed);

    service.denyAdoptRequest(4);
    mock.triggerError(mock.postCalls, serverErrorJson());

    QCOMPARE(denyFailed.count(), 1);
    QCOMPARE(createFailed.count(), 0);
    QCOMPARE(acceptFailed.count(), 0);
}

QTEST_MAIN(TestAdoptRequestService)

#include "adopt_request_service_test.moc"
