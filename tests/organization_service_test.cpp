#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QtTest>

#include "models/organization_dto.hpp"
#include "models/organization_register_dto.hpp"
#include "models/organization_update_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "services/organization_service.hpp"

using namespace pawspective::models;   // NOLINT google-build-using-namespace
using namespace pawspective::services; // NOLINT google-build-using-namespace

// ---------------------------------------------------------------------------
// Fake QNetworkReply that serves pre-baked data

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
    QList<Call> putCalls;

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
    void deleteResource(const QUrl&, CallbackHandler, CallbackHandler) override {}

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

static QByteArray validOrgJson(qint64 id = 1, const QString& name = "Test Org") {
    QJsonObject city;
    city["id"] = 1;
    city["name"] = "Moscow";

    QJsonObject org;
    org["id"] = id;
    org["name"] = name;
    org["city"] = city;

    return QJsonDocument(org).toJson(QJsonDocument::Compact);
}

static QByteArray serverErrorJson(const QString& message = "Not found") {
    QJsonObject err;
    err["message"] = message;
    return QJsonDocument(err).toJson(QJsonDocument::Compact);
}

static QByteArray validOrgListJson(int page = 1, int limit = 20, int totalCount = 1, int totalPages = 1) {
    QJsonObject city;
    city["id"] = 1;
    city["name"] = "Moscow";

    QJsonObject org;
    org["id"] = 1;
    org["name"] = "Test Org";
    org["city"] = city;

    QJsonArray items;
    items.append(org);

    QJsonObject root;
    root["items"] = items;
    root["page"] = page;
    root["limit"] = limit;
    root["total_count"] = totalCount;
    root["total_pages"] = totalPages;

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

// ---------------------------------------------------------------------------

class TestOrganizationService : public QObject {
    Q_OBJECT

private slots:
    // DTO round-trip tests (pre-existing, kept intact)
    void testOrganizationDtoFromJson_ValidObject();
    void testOrganizationDtoFromJson_MissingId_Throws();
    void testOrganizationDtoFromJson_MissingName_Throws();
    void testOrganizationDtoToJson_RoundTrip();
    void testOrganizationRegisterDtoToJson_RequiredFields();
    void testOrganizationRegisterDtoToJson_WithOptionalDescription();
    void testOrganizationUpdateDtoToJson_OnlySetFields();

    // Service signal tests
    void testGetOrganization_Success_EmitsGetOrganizationSuccess();
    void testGetOrganization_NetworkError_EmitsGetOrganizationFailed();
    void testGetOrganization_InvalidJson_EmitsGetOrganizationFailed();
    void testGetOrganization_ServerError_DoesNotEmitOtherFailedSignals();

    void testCreateOrganization_Success_EmitsCreateOrganizationSuccess();
    void testCreateOrganization_NetworkError_EmitsCreateOrganizationFailed();
    void testCreateOrganization_InvalidJson_EmitsCreateOrganizationFailed();
    void testCreateOrganization_ServerError_DoesNotEmitOtherFailedSignals();

    void testUpdateOrganization_Success_EmitsUpdateOrganizationSuccess();
    void testUpdateOrganization_NetworkError_EmitsUpdateOrganizationFailed();
    void testUpdateOrganization_InvalidJson_EmitsUpdateOrganizationFailed();
    void testUpdateOrganization_ServerError_DoesNotEmitOtherFailedSignals();

    // OrganizationListDTO tests
    void testOrganizationListDtoFromJson_ValidObject();

    // findByNameContaining signal tests
    void testFindByNameContaining_Success_EmitsFindByNameContainingSuccess();
    void testFindByNameContaining_Success_PaginationFieldsAreCorrect();
    void testFindByNameContaining_NetworkError_EmitsFindByNameContainingFailed();
    void testFindByNameContaining_InvalidJson_EmitsFindByNameContainingFailed();
    void testFindByNameContaining_EmptyName_EmitsFindByNameContainingFailed();
    void testFindByNameContaining_SendsPageAndLimitInQuery();
    void testFindByNameContaining_CustomPage_SendsCorrectPage();
    void testFindByNameContaining_ServerError_DoesNotEmitOtherFailedSignals();
};

// ---------------------------------------------------------------------------
// DTO tests (unchanged)

void TestOrganizationService::testOrganizationDtoFromJson_ValidObject(){
    QJsonObject cityJson;
    cityJson["id"] = 42;
    cityJson["name"] = "Moscow";

    QJsonObject json;
    json["id"] = 1;
    json["name"] = "Pawspective Org";
    json["description"] = "A pet shelter network";
    json["city"] = cityJson;

    OrganizationDTO dto = OrganizationDTO::fromJson(json);

    QCOMPARE(dto.id, static_cast<qint64>(1));
    QCOMPARE(dto.name, QString("Pawspective Org"));
    QVERIFY(dto.description.has_value());
    QCOMPARE(dto.description.value(), QString("A pet shelter network"));
    QCOMPARE(dto.city.id, static_cast<qint64>(42));
    QCOMPARE(dto.city.name, QString("Moscow"));
}

void TestOrganizationService::testOrganizationDtoFromJson_MissingId_Throws() {
    QJsonObject cityJson;
    cityJson["id"] = 1;
    cityJson["name"] = "City";

    QJsonObject json;
    json["name"] = "Org Without Id";
    json["city"] = cityJson;

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, OrganizationDTO::fromJson(json));
}

void TestOrganizationService::testOrganizationDtoFromJson_MissingName_Throws() {
    QJsonObject cityJson;
    cityJson["id"] = 1;
    cityJson["name"] = "City";

    QJsonObject json;
    json["id"] = 5;
    json["city"] = cityJson;

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, OrganizationDTO::fromJson(json));
}

void TestOrganizationService::testOrganizationDtoToJson_RoundTrip() {
    QJsonObject cityJson;
    cityJson["id"] = 7;
    cityJson["name"] = "Saint Petersburg";

    QJsonObject json;
    json["id"] = 3;
    json["name"] = "Happy Paws";
    json["city"] = cityJson;

    OrganizationDTO dto = OrganizationDTO::fromJson(json);
    QJsonObject result = dto.toJson();

    QCOMPARE(result["id"].toVariant().toLongLong(), static_cast<qint64>(3));
    QCOMPARE(result["name"].toString(), QString("Happy Paws"));
}

void TestOrganizationService::testOrganizationRegisterDtoToJson_RequiredFields() {
    OrganizationRegisterDTO dto;
    dto.name = "New Shelter";
    dto.cityId = 10;

    QJsonObject json = dto.toJson();

    QCOMPARE(json["name"].toString(), QString("New Shelter"));
    QCOMPARE(json["city_id"].toVariant().toLongLong(), static_cast<qint64>(10));
    QVERIFY(!json.contains("description"));
}

void TestOrganizationService::testOrganizationRegisterDtoToJson_WithOptionalDescription() {
    OrganizationRegisterDTO dto;
    dto.name = "New Shelter";
    dto.cityId = 10;
    dto.description = "We rescue cats and dogs";

    QJsonObject json = dto.toJson();

    QCOMPARE(json["description"].toString(), QString("We rescue cats and dogs"));
}

void TestOrganizationService::testOrganizationUpdateDtoToJson_OnlySetFields() {
    OrganizationUpdateDTO dto;
    dto.name = "Updated Name";

    QJsonObject json = dto.toJson();

    QCOMPARE(json["name"].toString(), QString("Updated Name"));
    QVERIFY(!json.contains("description"));
    QVERIFY(!json.contains("city"));
}

// ---------------------------------------------------------------------------
// getOrganization signal tests

void TestOrganizationService::testGetOrganization_Success_EmitsGetOrganizationSuccess() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::getOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::getOrganizationFailed);

    service.getOrganization(1);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validOrgJson(1, "Test Org"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto org = qvariant_cast<OrganizationDTO>(successSpy.at(0).at(0));
    QCOMPARE(org.id, static_cast<qint64>(1));
    QCOMPARE(org.name, QString("Test Org"));
}

void TestOrganizationService::testGetOrganization_NetworkError_EmitsGetOrganizationFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::getOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::getOrganizationFailed);

    service.getOrganization(1);
    mock.triggerError(mock.getCalls, serverErrorJson("Network error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testGetOrganization_InvalidJson_EmitsGetOrganizationFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::getOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::getOrganizationFailed);

    service.getOrganization(1);
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testGetOrganization_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy createFailed(&service, &OrganizationService::createOrganizationFailed);
    QSignalSpy updateFailed(&service, &OrganizationService::updateOrganizationFailed);
    QSignalSpy getFailed(&service, &OrganizationService::getOrganizationFailed);

    service.getOrganization(1);
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(getFailed.count(), 1);
    QCOMPARE(createFailed.count(), 0);
    QCOMPARE(updateFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// createOrganization signal tests

void TestOrganizationService::testCreateOrganization_Success_EmitsCreateOrganizationSuccess() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::createOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::createOrganizationFailed);

    OrganizationRegisterDTO dto;
    dto.name = "New Org";
    dto.cityId = 5;

    service.createOrganization(dto);
    QCOMPARE(mock.postCalls.size(), 1);

    mock.triggerSuccess(mock.postCalls, validOrgJson(42, "New Org"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto org = qvariant_cast<OrganizationDTO>(successSpy.at(0).at(0));
    QCOMPARE(org.id, static_cast<qint64>(42));
    QCOMPARE(org.name, QString("New Org"));
}

void TestOrganizationService::testCreateOrganization_NetworkError_EmitsCreateOrganizationFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::createOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::createOrganizationFailed);

    OrganizationRegisterDTO dto;
    dto.name = "New Org";
    dto.cityId = 5;

    service.createOrganization(dto);
    mock.triggerError(mock.postCalls, serverErrorJson("Conflict"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testCreateOrganization_InvalidJson_EmitsCreateOrganizationFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::createOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::createOrganizationFailed);

    OrganizationRegisterDTO dto;
    dto.name = "New Org";
    dto.cityId = 5;

    service.createOrganization(dto);
    mock.triggerSuccess(mock.postCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testCreateOrganization_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy getFailed(&service, &OrganizationService::getOrganizationFailed);
    QSignalSpy updateFailed(&service, &OrganizationService::updateOrganizationFailed);
    QSignalSpy createFailed(&service, &OrganizationService::createOrganizationFailed);

    OrganizationRegisterDTO dto;
    dto.name = "New Org";
    dto.cityId = 5;

    service.createOrganization(dto);
    mock.triggerError(mock.postCalls, serverErrorJson());

    QCOMPARE(createFailed.count(), 1);
    QCOMPARE(getFailed.count(), 0);
    QCOMPARE(updateFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// updateOrganization signal tests

void TestOrganizationService::testUpdateOrganization_Success_EmitsUpdateOrganizationSuccess() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::updateOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::updateOrganizationFailed);

    OrganizationUpdateDTO dto;
    dto.name = "Updated Name";

    service.updateOrganization(7, dto);
    QCOMPARE(mock.putCalls.size(), 1);

    mock.triggerSuccess(mock.putCalls, validOrgJson(7, "Updated Name"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto org = qvariant_cast<OrganizationDTO>(successSpy.at(0).at(0));
    QCOMPARE(org.id, static_cast<qint64>(7));
    QCOMPARE(org.name, QString("Updated Name"));
}

void TestOrganizationService::testUpdateOrganization_NetworkError_EmitsUpdateOrganizationFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::updateOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::updateOrganizationFailed);

    OrganizationUpdateDTO dto;
    dto.name = "Updated Name";

    service.updateOrganization(7, dto);
    mock.triggerError(mock.putCalls, serverErrorJson("Forbidden"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testUpdateOrganization_InvalidJson_EmitsUpdateOrganizationFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::updateOrganizationSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::updateOrganizationFailed);

    OrganizationUpdateDTO dto;
    dto.name = "Updated Name";

    service.updateOrganization(7, dto);
    mock.triggerSuccess(mock.putCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testUpdateOrganization_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy getFailed(&service, &OrganizationService::getOrganizationFailed);
    QSignalSpy createFailed(&service, &OrganizationService::createOrganizationFailed);
    QSignalSpy updateFailed(&service, &OrganizationService::updateOrganizationFailed);

    OrganizationUpdateDTO dto;
    dto.name = "Updated Name";

    service.updateOrganization(7, dto);
    mock.triggerError(mock.putCalls, serverErrorJson());

    QCOMPARE(updateFailed.count(), 1);
    QCOMPARE(getFailed.count(), 0);
    QCOMPARE(createFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// OrganizationListDTO tests

void TestOrganizationService::testOrganizationListDtoFromJson_ValidObject() {
    QJsonObject city;
    city["id"] = 1;
    city["name"] = "Moscow";

    QJsonObject org;
    org["id"] = 5;
    org["name"] = "Happy Paws";
    org["city"] = city;

    QJsonArray items;
    items.append(org);

    QJsonObject json;
    json["items"] = items;
    json["page"] = 2;
    json["limit"] = 20;
    json["total_count"] = 35;
    json["total_pages"] = 2;

    OrganizationListDTO dto = OrganizationListDTO::fromJson(json);

    QCOMPARE(dto.page, 2);
    QCOMPARE(dto.limit, 20);
    QCOMPARE(dto.totalCount, static_cast<qint64>(35));
    QCOMPARE(dto.totalPages, static_cast<qint64>(2));
    QCOMPARE(dto.items.size(), 1);
    QCOMPARE(dto.items.at(0).id, static_cast<qint64>(5));
    QCOMPARE(dto.items.at(0).name, QString("Happy Paws"));
}

// ---------------------------------------------------------------------------
// findByNameContaining signal tests

void TestOrganizationService::testFindByNameContaining_Success_EmitsFindByNameContainingSuccess() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::findByNameContainingSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::findByNameContainingFailed);

    service.findByNameContaining("Test");
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validOrgListJson());

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);
}

void TestOrganizationService::testFindByNameContaining_Success_PaginationFieldsAreCorrect() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::findByNameContainingSuccess);

    service.findByNameContaining("Test");
    mock.triggerSuccess(mock.getCalls, validOrgListJson(2, 20, 35, 2));

    QCOMPARE(successSpy.count(), 1);
    auto result = qvariant_cast<OrganizationListDTO>(successSpy.at(0).at(0));
    QCOMPARE(result.page, 2);
    QCOMPARE(result.limit, 20);
    QCOMPARE(result.totalCount, static_cast<qint64>(35));
    QCOMPARE(result.totalPages, static_cast<qint64>(2));
    QCOMPARE(result.items.size(), 1);
    QCOMPARE(result.items.at(0).name, QString("Test Org"));
}

void TestOrganizationService::testFindByNameContaining_NetworkError_EmitsFindByNameContainingFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::findByNameContainingSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::findByNameContainingFailed);

    service.findByNameContaining("Test");
    mock.triggerError(mock.getCalls, serverErrorJson("Network error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testFindByNameContaining_InvalidJson_EmitsFindByNameContainingFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::findByNameContainingSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::findByNameContainingFailed);

    service.findByNameContaining("Test");
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testFindByNameContaining_EmptyName_EmitsFindByNameContainingFailed() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy successSpy(&service, &OrganizationService::findByNameContainingSuccess);
    QSignalSpy failedSpy(&service, &OrganizationService::findByNameContainingFailed);

    service.findByNameContaining("");

    QCOMPARE(mock.getCalls.size(), 0);
    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testFindByNameContaining_SendsPageAndLimitInQuery() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    service.findByNameContaining("Test", 1, 20);
    QCOMPARE(mock.getCalls.size(), 1);

    const QUrlQuery query(mock.getCalls.at(0).endpoint.query());
    QCOMPARE(query.queryItemValue("name"), QString("Test"));
    QCOMPARE(query.queryItemValue("page"), QString("1"));
    QCOMPARE(query.queryItemValue("limit"), QString("20"));
}

void TestOrganizationService::testFindByNameContaining_CustomPage_SendsCorrectPage() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    service.findByNameContaining("Shelter", 3, 20);
    QCOMPARE(mock.getCalls.size(), 1);

    const QUrlQuery query(mock.getCalls.at(0).endpoint.query());
    QCOMPARE(query.queryItemValue("page"), QString("3"));
    QCOMPARE(query.queryItemValue("name"), QString("Shelter"));
}

void TestOrganizationService::testFindByNameContaining_ServerError_DoesNotEmitOtherFailedSignals() {
    MockNetworkClient mock;
    OrganizationService service(mock);

    QSignalSpy getFailed(&service, &OrganizationService::getOrganizationFailed);
    QSignalSpy createFailed(&service, &OrganizationService::createOrganizationFailed);
    QSignalSpy updateFailed(&service, &OrganizationService::updateOrganizationFailed);
    QSignalSpy findFailed(&service, &OrganizationService::findByNameContainingFailed);

    service.findByNameContaining("Test");
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(findFailed.count(), 1);
    QCOMPARE(getFailed.count(), 0);
    QCOMPARE(createFailed.count(), 0);
    QCOMPARE(updateFailed.count(), 0);
}

QTEST_MAIN(TestOrganizationService)

#include "organization_service_test.moc"

