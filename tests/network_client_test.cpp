#include <QtTest/qsignalspy.h>
#include <QtTest/qtest.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "services/network_client.hpp"

using namespace pawspective::services; //  NOLINT google-build-using-namespace

class TestNetworkClient : public QObject {
    Q_OBJECT


    NetworkClient* m_client = nullptr;

private slots:
    void init();
    void cleanup();

    void testGetRequest();
    void testPostRequest();
    void testPutRequest();
    void testPatchRequest();
    void testDeleteRequest();

    void testNetworkError();
    void testSuccessCallback();
    void testErrorCallback();

    void testUnauthorizedAccessSignal();
    void testPendingRequestsRetry();

};

void TestNetworkClient::init() {
    m_client = new NetworkClient();
    QVERIFY(m_client != nullptr);
}

void TestNetworkClient::cleanup() {
    delete m_client;
    m_client = nullptr;
}

void TestNetworkClient::testGetRequest() {
    QVERIFY(m_client != nullptr);

    bool callbackExecuted = false;
    auto onSuccess = [&callbackExecuted](const QNetworkReply&) {
        callbackExecuted = true;
    };
    auto onError = [](const QNetworkReply&) {};

    QUrl endpoint("http://example.com/api/test");
    m_client->get(endpoint, onSuccess, onError);

    QTest::qWait(100);
}

void TestNetworkClient::testPostRequest() {
    QVERIFY(m_client != nullptr);

    bool callbackExecuted = false;
    auto onSuccess = [&callbackExecuted](const QNetworkReply&) {
        callbackExecuted = true;
    };
    auto onError = [](const QNetworkReply&) {};

    QUrl endpoint("http://example.com/api/users");
    QByteArray data = R"({"name": "test", "email": "test@example.com"})";

    m_client->post(endpoint, data, onSuccess, onError);

    QTest::qWait(100);
}

void TestNetworkClient::testPutRequest() {
    QVERIFY(m_client != nullptr);

    auto onSuccess = [](const QNetworkReply&) {};
    auto onError = [](const QNetworkReply&) {};

    QUrl endpoint("http://example.com/api/users/1");
    QByteArray data = R"({"name": "updated"})";

    m_client->put(endpoint, data, onSuccess, onError);

    QTest::qWait(100);
}

void TestNetworkClient::testPatchRequest() {
    QVERIFY(m_client != nullptr);

    auto onSuccess = [](const QNetworkReply&) {};
    auto onError = [](const QNetworkReply&) {
        QFAIL("Error callback should not be called for patch request");
    };

    QUrl endpoint("http://example.com/api/users/1");
    QByteArray data = R"({"status": "active"})";

    m_client->patch(endpoint, data, onSuccess, onError);

    QTest::qWait(100);
}

void TestNetworkClient::testDeleteRequest() {
    QVERIFY(m_client != nullptr);

    auto onSuccess = [](const QNetworkReply&) {};
    auto onError = [](const QNetworkReply&) {};

    QUrl endpoint("http://example.com/api/users/1");

    m_client->deleteResource(endpoint, onSuccess, onError);

    QTest::qWait(100);
}

void TestNetworkClient::testNetworkError() {
    QVERIFY(m_client != nullptr);

    bool errorCallbackExecuted = false;
    auto onSuccess = [](const QNetworkReply&) {
        QFAIL("Success callback should not be called on error");
    };
    auto onError = [&errorCallbackExecuted](const QNetworkReply&) {
        errorCallbackExecuted = true;
    };

    QUrl invalidEndpoint("http://invalid.endpoint.local.test/api/test");
    m_client->get(invalidEndpoint, onSuccess, onError);

    QTest::qWait(200);
}

void TestNetworkClient::testSuccessCallback() {
    QVERIFY(m_client != nullptr);

    bool callbackExecuted = false;
    int callCount = 0;

    auto onSuccess = [&callbackExecuted, &callCount](const QNetworkReply&) {
        callbackExecuted = true;
        callCount++;
    };
    auto onError = [](const QNetworkReply&) {};

    QUrl endpoint("http://httpbin.org/get");
    m_client->get(endpoint, onSuccess, onError);
    QTest::qWait(3000);
    QCOMPARE(callCount, 1);
}

void TestNetworkClient::testErrorCallback() {
    QVERIFY(m_client != nullptr);

    bool errorCallbackExecuted = false;
    int errorCallCount = 0;

    auto onSuccess = [](const QNetworkReply&) {
        QFAIL("Success callback should not be called on error");
    };
    auto onError = [&errorCallbackExecuted, &errorCallCount](const QNetworkReply&) {
        errorCallbackExecuted = true;
        errorCallCount++;
    };

    QUrl endpoint("http://this-domain-definitely-does-not-exist-12345.test/api/test");
    m_client->get(endpoint, onSuccess, onError);

    QTest::qWait(6000);

    QVERIFY(errorCallbackExecuted || errorCallCount > 0);
}

void TestNetworkClient::testUnauthorizedAccessSignal() {
    QVERIFY(m_client != nullptr);

    QSignalSpy unauthorizedSpy(m_client, &NetworkClient::unauthorizedAccess);

    auto onSuccess = [](const QNetworkReply&) {};
    auto onError = [](const QNetworkReply&) {};
    QUrl endpoint("http://example.com/api/protected");
    m_client->get(endpoint, onSuccess, onError);

    QTest::qWait(100);

    QVERIFY(unauthorizedSpy.isValid());
}

void TestNetworkClient::testPendingRequestsRetry() {
    QVERIFY(m_client != nullptr);

    int successCount = 0;
    auto onSuccess = [&successCount](const QNetworkReply&) {
        successCount++;
    };
    auto onError = [](const QNetworkReply&) {};

    QUrl endpoint("http://example.com/api/test");
    m_client->get(endpoint, onSuccess, onError);

    QTest::qWait(100);

    m_client->retryPendingRequests();

    QTest::qWait(100);
}

QTEST_MAIN(TestNetworkClient)

#include "network_client_test.moc"
