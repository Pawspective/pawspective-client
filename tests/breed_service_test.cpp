#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QtTest>

#include "models/animal_enums.hpp"
#include "models/breed_dto.hpp"
#include "services/breed_service.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"

using namespace pawspective::models;   // NOLINT google-build-using-namespace
using namespace pawspective::services; // NOLINT google-build-using-namespace

// ---------------------------------------------------------------------------
// Fake QNetworkReply

class FakeNetworkReply : public QNetworkReply {
    Q_OBJECT
public:
    explicit FakeNetworkReply(const QByteArray& data, QObject* parent = nullptr)
        : QNetworkReply(parent), m_data(data) {
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
// MockNetworkClient

class MockNetworkClient : public INetworkClient {
public:
    using CallbackHandler = INetworkClient::CallbackHandler;

    struct Call {
        QUrl endpoint;
        CallbackHandler onSuccess;
        CallbackHandler onError;
    };

    QList<Call> getCalls;

    void get(const QUrl& url, CallbackHandler ok, CallbackHandler err) override {
        getCalls.append({url, ok, err});
    }
    void post(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void put(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void patch(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void deleteResource(const QUrl&, CallbackHandler, CallbackHandler) override {}

    void triggerSuccess(const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        getCalls[idx].onSuccess(reply);
    }
    void triggerError(const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        getCalls[idx].onError(reply);
    }
};

// ---------------------------------------------------------------------------
// Helpers

static QByteArray validBreedArrayJson() {
    QJsonObject dog1;
    dog1["id"] = 1;
    dog1["animalType"] = "DOG";
    dog1["name"] = "Labrador";

    QJsonObject dog2;
    dog2["id"] = 2;
    dog2["animalType"] = "DOG";
    dog2["name"] = "Poodle";

    return QJsonDocument(QJsonArray{dog1, dog2}).toJson(QJsonDocument::Compact);
}

static QByteArray serverErrorJson(const QString& message = "Not found") {
    QJsonObject err;
    err["message"] = message;
    return QJsonDocument(err).toJson(QJsonDocument::Compact);
}

// ---------------------------------------------------------------------------

class TestBreedService : public QObject {
    Q_OBJECT

private slots:
    void testGetBreedsByType_Success_EmitsGetBreedsByTypeSuccess();
    void testGetBreedsByType_NetworkError_EmitsGetBreedsByTypeFailed();
    void testGetBreedsByType_InvalidJson_EmitsGetBreedsByTypeFailed();
    void testGetBreedsByType_ServerError_DoesNotEmitSuccess();
    void testGetBreedsByType_UsesCorrectQueryParam();
};

// ---------------------------------------------------------------------------

void TestBreedService::testGetBreedsByType_Success_EmitsGetBreedsByTypeSuccess() {
    MockNetworkClient mock;
    BreedService service(mock);

    QSignalSpy successSpy(&service, &BreedService::getBreedsByTypeSuccess);
    QSignalSpy failedSpy(&service, &BreedService::getBreedsByTypeFailed);

    service.getBreedsByType(AnimalType::Dog);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(validBreedArrayJson());

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto breeds = qvariant_cast<QList<BreedDTO>>(successSpy.at(0).at(0));
    QCOMPARE(breeds.size(), 2);
    QCOMPARE(breeds[0].name, QString("Labrador"));
    QCOMPARE(breeds[1].name, QString("Poodle"));
}

void TestBreedService::testGetBreedsByType_NetworkError_EmitsGetBreedsByTypeFailed() {
    MockNetworkClient mock;
    BreedService service(mock);

    QSignalSpy successSpy(&service, &BreedService::getBreedsByTypeSuccess);
    QSignalSpy failedSpy(&service, &BreedService::getBreedsByTypeFailed);

    service.getBreedsByType(AnimalType::Cat);
    mock.triggerError(serverErrorJson("Internal server error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestBreedService::testGetBreedsByType_InvalidJson_EmitsGetBreedsByTypeFailed() {
    MockNetworkClient mock;
    BreedService service(mock);

    QSignalSpy successSpy(&service, &BreedService::getBreedsByTypeSuccess);
    QSignalSpy failedSpy(&service, &BreedService::getBreedsByTypeFailed);

    service.getBreedsByType(AnimalType::Dog);
    mock.triggerSuccess(QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestBreedService::testGetBreedsByType_ServerError_DoesNotEmitSuccess() {
    MockNetworkClient mock;
    BreedService service(mock);

    QSignalSpy successSpy(&service, &BreedService::getBreedsByTypeSuccess);
    QSignalSpy failedSpy(&service, &BreedService::getBreedsByTypeFailed);

    service.getBreedsByType(AnimalType::Other);
    mock.triggerError(serverErrorJson());

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestBreedService::testGetBreedsByType_UsesCorrectQueryParam() {
    MockNetworkClient mock;
    BreedService service(mock);

    service.getBreedsByType(AnimalType::Dog);

    QCOMPARE(mock.getCalls.size(), 1);
    QUrlQuery query(mock.getCalls[0].endpoint);
    QVERIFY(mock.getCalls[0].endpoint.path().contains("/breed"));
    QCOMPARE(query.queryItemValue("type"), toApiString(AnimalType::Dog));
}

QTEST_MAIN(TestBreedService)

#include "breed_service_test.moc"
