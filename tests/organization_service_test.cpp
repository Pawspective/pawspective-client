#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

#include "models/organization_dto.hpp"
#include "models/organization_register_dto.hpp"
#include "models/organization_update_dto.hpp"
#include "services/errors.hpp"
#include "services/network_client.hpp"
#include "services/organization_service.hpp"

using namespace pawspective::services;  //  NOLINT google-build-using-namespace
using namespace pawspective::models;    //  NOLINT google-build-using-namespace

class TestOrganizationService : public QObject {
    Q_OBJECT

    NetworkClient* m_networkClient = nullptr;
    OrganizationService* m_service = nullptr;

private slots:
    void init();
    void cleanup();

    void testServiceConstruction();
    void testAllSignalsAreValid();

    void testGetOrganization_EmitsRequestFailedOnConnectionError();
    void testCreateOrganization_EmitsRequestFailedOnConnectionError();
    void testUpdateOrganization_EmitsRequestFailedOnConnectionError();

    void testOrganizationDtoFromJson_ValidObject();
    void testOrganizationDtoFromJson_MissingId_Throws();
    void testOrganizationDtoFromJson_MissingName_Throws();
    void testOrganizationDtoToJson_RoundTrip();

    void testOrganizationRegisterDtoToJson_RequiredFields();
    void testOrganizationRegisterDtoToJson_WithOptionalDescription();

    void testOrganizationUpdateDtoToJson_OnlySetFields();
};

void TestOrganizationService::init() {
    m_networkClient = new NetworkClient();
    m_service = new OrganizationService(*m_networkClient);
    QVERIFY(m_networkClient != nullptr);
    QVERIFY(m_service != nullptr);
}

void TestOrganizationService::cleanup() {
    delete m_service;
    m_service = nullptr;
    delete m_networkClient;
    m_networkClient = nullptr;
}

void TestOrganizationService::testServiceConstruction() { QVERIFY(m_service != nullptr); }

void TestOrganizationService::testAllSignalsAreValid() {
    QSignalSpy getSuccessSpy(m_service, &OrganizationService::getOrganizationSuccess);
    QSignalSpy createSuccessSpy(m_service, &OrganizationService::createOrganizationSuccess);
    QSignalSpy updateSuccessSpy(m_service, &OrganizationService::updateOrganizationSuccess);
    QSignalSpy failedSpy(m_service, &OrganizationService::requestFailed);

    QVERIFY(getSuccessSpy.isValid());
    QVERIFY(createSuccessSpy.isValid());
    QVERIFY(updateSuccessSpy.isValid());
    QVERIFY(failedSpy.isValid());
}

void TestOrganizationService::testGetOrganization_EmitsRequestFailedOnConnectionError() {
    QSignalSpy failedSpy(m_service, &OrganizationService::requestFailed);
    QVERIFY(failedSpy.isValid());

    m_service->getOrganization(1);

    QTRY_VERIFY_WITH_TIMEOUT(failedSpy.count() > 0, 6000);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testCreateOrganization_EmitsRequestFailedOnConnectionError() {
    QSignalSpy failedSpy(m_service, &OrganizationService::requestFailed);
    QVERIFY(failedSpy.isValid());

    OrganizationRegisterDTO dto;
    dto.name = "Test Org";
    dto.cityId = 1;
    m_service->createOrganization(dto);

    QTRY_VERIFY_WITH_TIMEOUT(failedSpy.count() > 0, 6000);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testUpdateOrganization_EmitsRequestFailedOnConnectionError() {
    QSignalSpy failedSpy(m_service, &OrganizationService::requestFailed);
    QVERIFY(failedSpy.isValid());

    OrganizationUpdateDTO dto;
    dto.name = "Updated Org";
    m_service->updateOrganization(1, dto);

    QTRY_VERIFY_WITH_TIMEOUT(failedSpy.count() > 0, 6000);
    QCOMPARE(failedSpy.count(), 1);
}

void TestOrganizationService::testOrganizationDtoFromJson_ValidObject() {
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

QTEST_MAIN(TestOrganizationService)

#include "organization_service_test.moc"
