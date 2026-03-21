#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QtTest>

#include "models/animal_dto.hpp"
#include "models/animal_filter_dto.hpp"
#include "models/animal_register_dto.hpp"
#include "models/animal_update_dto.hpp"
#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "services/animal_service.hpp"

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

static QJsonObject breedJson(qint64 id = 1, const QString& type = "dog", const QString& name = "Labrador") {
    QJsonObject breed;
    breed["id"] = id;
    breed["animal_type"] = type;
    breed["name"] = name;
    return breed;
}

static QByteArray validAnimalJson(qint64 id = 1, const QString& name = "Buddy") {
    QJsonObject animal;
    animal["id"] = id;
    animal["organization_id"] = 10;
    animal["name"] = name;
    animal["breed"] = breedJson();
    animal["size"] = "medium";
    animal["gender"] = "male";
    animal["care_level"] = "easy";
    animal["color"] = "black";
    animal["good_with"] = "dogs";
    animal["age"] = 3;
    animal["status"] = "available";
    return QJsonDocument(animal).toJson(QJsonDocument::Compact);
}

static QByteArray validAnimalArrayJson() {
    QJsonArray arr;

    QJsonObject a1;
    a1["id"] = 1;
    a1["organization_id"] = 10;
    a1["name"] = "Buddy";
    a1["breed"] = breedJson(1, "dog", "Labrador");
    a1["size"] = "medium";
    a1["gender"] = "male";
    a1["care_level"] = "easy";
    a1["color"] = "black";
    a1["good_with"] = "dogs";
    a1["age"] = 3;
    a1["status"] = "available";

    QJsonObject a2;
    a2["id"] = 2;
    a2["organization_id"] = 10;
    a2["name"] = "Whiskers";
    a2["breed"] = breedJson(2, "cat", "Siamese");
    a2["size"] = "small";
    a2["gender"] = "female";
    a2["care_level"] = "moderate";
    a2["color"] = "white";
    a2["good_with"] = "cats";
    a2["age"] = 2;
    a2["status"] = "available";

    arr.append(a1);
    arr.append(a2);
    return QJsonDocument(arr).toJson(QJsonDocument::Compact);
}

static QByteArray validFilterJson() {
    QJsonObject filter;
    QJsonArray breeds;
    breeds.append(1);
    breeds.append(2);
    filter["breeds"] = breeds;
    QJsonArray sizes;
    sizes.append("small");
    sizes.append("medium");
    filter["sizes"] = sizes;
    filter["age_lte"] = 5;
    filter["age_gte"] = 1;
    return QJsonDocument(filter).toJson(QJsonDocument::Compact);
}

static QByteArray serverErrorJson(const QString& message = "Not found") {
    QJsonObject err;
    err["message"] = message;
    return QJsonDocument(err).toJson(QJsonDocument::Compact);
}

// ---------------------------------------------------------------------------

class TestAnimalService : public QObject {
    Q_OBJECT

private slots:
    // DTO round-trip tests
    void testAnimalDtoFromJson_ValidObject();
    void testAnimalDtoFromJson_MissingId_Throws();
    void testAnimalDtoFromJson_MissingName_Throws();
    void testAnimalDtoToJson_RoundTrip();
    void testAnimalDtoToJson_WithDescription();
    void testAnimalFilterDtoFromJson_ValidObject();
    void testAnimalFilterDtoFromJson_EmptyObject();
    void testAnimalRegisterDtoToJson_RequiredFields();
    void testAnimalUpdateDtoToJson_OnlySetFields();

    // getAnimals signal tests
    void testGetAnimals_Success_EmitsGetAnimalsSuccess();
    void testGetAnimals_NetworkError_EmitsGetAnimalsFailed();
    void testGetAnimals_InvalidJson_EmitsGetAnimalsFailed();
    void testGetAnimals_ServerError_DoesNotEmitOtherSignals();
    void testGetAnimals_WithFilter_BuildsQueryParams();

    // getAnimal signal tests
    void testGetAnimal_Success_EmitsGetAnimalSuccess();
    void testGetAnimal_NetworkError_EmitsGetAnimalFailed();
    void testGetAnimal_InvalidJson_EmitsGetAnimalFailed();
    void testGetAnimal_ServerError_DoesNotEmitOtherSignals();

    // createAnimal signal tests
    void testCreateAnimal_Success_EmitsCreateAnimalSuccess();
    void testCreateAnimal_NetworkError_EmitsCreateAnimalFailed();
    void testCreateAnimal_InvalidJson_EmitsCreateAnimalFailed();
    void testCreateAnimal_ServerError_DoesNotEmitOtherSignals();

    // updateAnimal signal tests
    void testUpdateAnimal_Success_EmitsUpdateAnimalSuccess();
    void testUpdateAnimal_NetworkError_EmitsUpdateAnimalFailed();
    void testUpdateAnimal_InvalidJson_EmitsUpdateAnimalFailed();
    void testUpdateAnimal_ServerError_DoesNotEmitOtherSignals();

    // getAnimalFilters signal tests
    void testGetAnimalFilters_Success_EmitsGetAnimalFiltersSuccess();
    void testGetAnimalFilters_NetworkError_EmitsGetAnimalFiltersFailed();
    void testGetAnimalFilters_InvalidJson_EmitsGetAnimalFiltersFailed();
    void testGetAnimalFilters_ServerError_DoesNotEmitOtherSignals();

    // getAnimalsByOrganization signal tests
    void testGetAnimalsByOrganization_Success_EmitsGetAnimalsByOrganizationSuccess();
    void testGetAnimalsByOrganization_NetworkError_EmitsGetAnimalsByOrganizationFailed();
    void testGetAnimalsByOrganization_InvalidJson_EmitsGetAnimalsByOrganizationFailed();
    void testGetAnimalsByOrganization_ServerError_DoesNotEmitOtherSignals();
    void testGetAnimalsByOrganization_UsesCorrectEndpoint();
};

// ---------------------------------------------------------------------------
// DTO tests

void TestAnimalService::testAnimalDtoFromJson_ValidObject() {
    QJsonObject json;
    json["id"] = 5;
    json["organization_id"] = 10;
    json["name"] = "Rex";
    json["breed"] = breedJson(3, "dog", "German Shepherd");
    json["size"] = "large";
    json["gender"] = "male";
    json["care_level"] = "moderate";
    json["color"] = "brown";
    json["good_with"] = "children";
    json["age"] = 4;
    json["status"] = "available";

    AnimalDTO dto = AnimalDTO::fromJson(json);

    QCOMPARE(dto.id, static_cast<qint64>(5));
    QCOMPARE(dto.organizationId, static_cast<qint64>(10));
    QCOMPARE(dto.name, QString("Rex"));
    QCOMPARE(dto.breed.id, static_cast<qint64>(3));
    QCOMPARE(dto.breed.name, QString("German Shepherd"));
    QCOMPARE(dto.size, AnimalSize::Large);
    QCOMPARE(dto.gender, AnimalGender::Male);
    QCOMPARE(dto.careLevel, CareLevel::Moderate);
    QCOMPARE(dto.color, AnimalColor::Brown);
    QCOMPARE(dto.goodWith, GoodWith::Children);
    QCOMPARE(dto.age, 4);
    QCOMPARE(dto.status, AnimalStatus::Available);
    QVERIFY(!dto.description.has_value());
}

void TestAnimalService::testAnimalDtoFromJson_MissingId_Throws() {
    QJsonObject json;
    json["organization_id"] = 10;
    json["name"] = "NoId";
    json["breed"] = breedJson();
    json["size"] = "small";
    json["gender"] = "female";
    json["care_level"] = "easy";
    json["color"] = "white";
    json["good_with"] = "cats";
    json["age"] = 1;
    json["status"] = "available";

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, AnimalDTO::fromJson(json));
}

void TestAnimalService::testAnimalDtoFromJson_MissingName_Throws() {
    QJsonObject json;
    json["id"] = 1;
    json["organization_id"] = 10;
    json["breed"] = breedJson();
    json["size"] = "small";
    json["gender"] = "female";
    json["care_level"] = "easy";
    json["color"] = "white";
    json["good_with"] = "cats";
    json["age"] = 1;
    json["status"] = "available";

    QVERIFY_THROWS_EXCEPTION(std::invalid_argument, AnimalDTO::fromJson(json));
}

void TestAnimalService::testAnimalDtoToJson_RoundTrip() {
    QJsonObject json;
    json["id"] = 7;
    json["organization_id"] = 3;
    json["name"] = "Luna";
    json["breed"] = breedJson(2, "cat", "Persian");
    json["size"] = "small";
    json["gender"] = "female";
    json["care_level"] = "easy";
    json["color"] = "white";
    json["good_with"] = "elderly";
    json["age"] = 2;
    json["status"] = "available";

    AnimalDTO dto = AnimalDTO::fromJson(json);
    QJsonObject result = dto.toJson();

    QCOMPARE(result["id"].toVariant().toLongLong(), static_cast<qint64>(7));
    QCOMPARE(result["name"].toString(), QString("Luna"));
    QCOMPARE(result["size"].toString(), QString("small"));
    QCOMPARE(result["gender"].toString(), QString("female"));
    QVERIFY(!result.contains("description"));
}

void TestAnimalService::testAnimalDtoToJson_WithDescription() {
    QJsonObject json;
    json["id"] = 8;
    json["organization_id"] = 3;
    json["name"] = "Max";
    json["breed"] = breedJson(1, "dog", "Poodle");
    json["size"] = "small";
    json["gender"] = "male";
    json["care_level"] = "easy";
    json["color"] = "white";
    json["good_with"] = "dogs";
    json["age"] = 1;
    json["description"] = "Very friendly";
    json["status"] = "available";

    AnimalDTO dto = AnimalDTO::fromJson(json);
    QJsonObject result = dto.toJson();

    QVERIFY(dto.description.has_value());
    QCOMPARE(dto.description.value(), QString("Very friendly"));
    QCOMPARE(result["description"].toString(), QString("Very friendly"));
}

void TestAnimalService::testAnimalFilterDtoFromJson_ValidObject() {
    QJsonArray breeds;
    breeds.append(1);
    breeds.append(2);
    QJsonArray sizes;
    sizes.append("small");
    sizes.append("medium");

    QJsonObject json;
    json["breeds"] = breeds;
    json["sizes"] = sizes;
    json["age_lte"] = 5;
    json["age_gte"] = 1;

    AnimalFilterDTO dto = AnimalFilterDTO::fromJson(json);

    QVERIFY(dto.breeds.has_value());
    QCOMPARE(dto.breeds->size(), 2);
    QCOMPARE((*dto.breeds)[0], static_cast<int64_t>(1));
    QVERIFY(dto.sizes.has_value());
    QCOMPARE(dto.sizes->size(), 2);
    QCOMPARE((*dto.sizes)[0], AnimalSize::Small);
    QVERIFY(dto.ageLte.has_value());
    QCOMPARE(*dto.ageLte, 5);
    QVERIFY(dto.ageGte.has_value());
    QCOMPARE(*dto.ageGte, 1);
}

void TestAnimalService::testAnimalFilterDtoFromJson_EmptyObject() {
    AnimalFilterDTO dto = AnimalFilterDTO::fromJson(QJsonObject{});

    QVERIFY(!dto.breeds.has_value());
    QVERIFY(!dto.animalTypes.has_value());
    QVERIFY(!dto.sizes.has_value());
    QVERIFY(!dto.genders.has_value());
    QVERIFY(!dto.ageLte.has_value());
    QVERIFY(!dto.ageGte.has_value());
}

void TestAnimalService::testAnimalRegisterDtoToJson_RequiredFields() {
    AnimalRegisterDTO dto;
    dto.organizationId = 1;
    dto.name = "Buddy";
    dto.breedId = 3;
    dto.size = AnimalSize::Medium;
    dto.gender = AnimalGender::Male;
    dto.careLevel = CareLevel::Easy;
    dto.color = AnimalColor::Black;
    dto.goodWith = GoodWith::Dogs;
    dto.age = 2;
    dto.status = AnimalStatus::Available;

    QJsonObject json = dto.toJson();

    QCOMPARE(json["name"].toString(), QString("Buddy"));
    QCOMPARE(json["size"].toString(), QString("medium"));
    QCOMPARE(json["gender"].toString(), QString("male"));
    QVERIFY(!json.contains("description"));
}

void TestAnimalService::testAnimalUpdateDtoToJson_OnlySetFields() {
    AnimalUpdateDTO dto;
    dto.name = "Updated Name";
    dto.age = 4;

    QJsonObject json = dto.toJson();

    QCOMPARE(json["name"].toString(), QString("Updated Name"));
    QCOMPARE(json["age"].toInt(), 4);
    QVERIFY(!json.contains("size"));
    QVERIFY(!json.contains("gender"));
    QVERIFY(!json.contains("description"));
}

// ---------------------------------------------------------------------------
// getAnimals signal tests

void TestAnimalService::testGetAnimals_Success_EmitsGetAnimalsSuccess() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalsSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalsFailed);

    service.getAnimals(AnimalFilterDTO{});
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validAnimalArrayJson());

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto animals = qvariant_cast<QList<AnimalDTO>>(successSpy.at(0).at(0));
    QCOMPARE(animals.size(), 2);
    QCOMPARE(animals[0].name, QString("Buddy"));
    QCOMPARE(animals[1].name, QString("Whiskers"));
}

void TestAnimalService::testGetAnimals_NetworkError_EmitsGetAnimalsFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalsSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalsFailed);

    service.getAnimals(AnimalFilterDTO{});
    mock.triggerError(mock.getCalls, serverErrorJson("Server error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimals_InvalidJson_EmitsGetAnimalsFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalsSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalsFailed);

    service.getAnimals(AnimalFilterDTO{});
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimals_ServerError_DoesNotEmitOtherSignals() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy getAnimalFailed(&service, &AnimalService::getAnimalFailed);
    QSignalSpy createFailed(&service, &AnimalService::createAnimalFailed);
    QSignalSpy getAnimalsFailed(&service, &AnimalService::getAnimalsFailed);

    service.getAnimals(AnimalFilterDTO{});
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(getAnimalsFailed.count(), 1);
    QCOMPARE(getAnimalFailed.count(), 0);
    QCOMPARE(createFailed.count(), 0);
}

void TestAnimalService::testGetAnimals_WithFilter_BuildsQueryParams() {
    MockNetworkClient mock;
    AnimalService service(mock);

    AnimalFilterDTO filter;
    filter.sizes = QVector<AnimalSize>{AnimalSize::Small, AnimalSize::Medium};
    filter.genders = QVector<AnimalGender>{AnimalGender::Female};
    filter.ageGte = 1;
    filter.ageLte = 5;

    service.getAnimals(filter);

    QCOMPARE(mock.getCalls.size(), 1);
    QString query = mock.getCalls[0].endpoint.query();
    QVERIFY(query.contains("size=small"));
    QVERIFY(query.contains("size=medium"));
    QVERIFY(query.contains("gender=female"));
    QVERIFY(query.contains("ageGte=1"));
    QVERIFY(query.contains("ageLte=5"));
}

// ---------------------------------------------------------------------------
// getAnimal signal tests

void TestAnimalService::testGetAnimal_Success_EmitsGetAnimalSuccess() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalFailed);

    service.getAnimal(1);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validAnimalJson(1, "Buddy"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto animal = qvariant_cast<AnimalDTO>(successSpy.at(0).at(0));
    QCOMPARE(animal.id, static_cast<qint64>(1));
    QCOMPARE(animal.name, QString("Buddy"));
}

void TestAnimalService::testGetAnimal_NetworkError_EmitsGetAnimalFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalFailed);

    service.getAnimal(1);
    mock.triggerError(mock.getCalls, serverErrorJson("Not found"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimal_InvalidJson_EmitsGetAnimalFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalFailed);

    service.getAnimal(1);
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimal_ServerError_DoesNotEmitOtherSignals() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy getAnimalsFailed(&service, &AnimalService::getAnimalsFailed);
    QSignalSpy createFailed(&service, &AnimalService::createAnimalFailed);
    QSignalSpy getAnimalFailed(&service, &AnimalService::getAnimalFailed);

    service.getAnimal(1);
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(getAnimalFailed.count(), 1);
    QCOMPARE(getAnimalsFailed.count(), 0);
    QCOMPARE(createFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// createAnimal signal tests

void TestAnimalService::testCreateAnimal_Success_EmitsCreateAnimalSuccess() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::createAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::createAnimalFailed);

    AnimalRegisterDTO dto;
    dto.organizationId = 1;
    dto.name = "Buddy";
    dto.breedId = 3;
    dto.size = AnimalSize::Medium;
    dto.gender = AnimalGender::Male;
    dto.careLevel = CareLevel::Easy;
    dto.color = AnimalColor::Black;
    dto.goodWith = GoodWith::Dogs;
    dto.age = 2;
    dto.status = AnimalStatus::Available;

    service.createAnimal(dto);
    QCOMPARE(mock.postCalls.size(), 1);

    mock.triggerSuccess(mock.postCalls, validAnimalJson(99, "Buddy"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto animal = qvariant_cast<AnimalDTO>(successSpy.at(0).at(0));
    QCOMPARE(animal.id, static_cast<qint64>(99));
    QCOMPARE(animal.name, QString("Buddy"));
}

void TestAnimalService::testCreateAnimal_NetworkError_EmitsCreateAnimalFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::createAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::createAnimalFailed);

    AnimalRegisterDTO dto;
    dto.organizationId = 1;
    dto.name = "Buddy";
    dto.breedId = 3;
    dto.size = AnimalSize::Medium;
    dto.gender = AnimalGender::Male;
    dto.careLevel = CareLevel::Easy;
    dto.color = AnimalColor::Black;
    dto.goodWith = GoodWith::Dogs;
    dto.age = 2;
    dto.status = AnimalStatus::Available;

    service.createAnimal(dto);
    mock.triggerError(mock.postCalls, serverErrorJson("Conflict"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testCreateAnimal_InvalidJson_EmitsCreateAnimalFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::createAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::createAnimalFailed);

    AnimalRegisterDTO dto;
    dto.organizationId = 1;
    dto.name = "Buddy";
    dto.breedId = 3;
    dto.size = AnimalSize::Medium;
    dto.gender = AnimalGender::Male;
    dto.careLevel = CareLevel::Easy;
    dto.color = AnimalColor::Black;
    dto.goodWith = GoodWith::Dogs;
    dto.age = 2;
    dto.status = AnimalStatus::Available;

    service.createAnimal(dto);
    mock.triggerSuccess(mock.postCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testCreateAnimal_ServerError_DoesNotEmitOtherSignals() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy getAnimalFailed(&service, &AnimalService::getAnimalFailed);
    QSignalSpy updateFailed(&service, &AnimalService::updateAnimalFailed);
    QSignalSpy createFailed(&service, &AnimalService::createAnimalFailed);

    AnimalRegisterDTO dto;
    dto.organizationId = 1;
    dto.name = "Buddy";
    dto.breedId = 3;
    dto.size = AnimalSize::Medium;
    dto.gender = AnimalGender::Male;
    dto.careLevel = CareLevel::Easy;
    dto.color = AnimalColor::Black;
    dto.goodWith = GoodWith::Dogs;
    dto.age = 2;
    dto.status = AnimalStatus::Available;

    service.createAnimal(dto);
    mock.triggerError(mock.postCalls, serverErrorJson());

    QCOMPARE(createFailed.count(), 1);
    QCOMPARE(getAnimalFailed.count(), 0);
    QCOMPARE(updateFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// updateAnimal signal tests

void TestAnimalService::testUpdateAnimal_Success_EmitsUpdateAnimalSuccess() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::updateAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::updateAnimalFailed);

    AnimalUpdateDTO dto;
    dto.name = "Updated Buddy";

    service.updateAnimal(1, dto);
    QCOMPARE(mock.putCalls.size(), 1);

    mock.triggerSuccess(mock.putCalls, validAnimalJson(1, "Updated Buddy"));

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto animal = qvariant_cast<AnimalDTO>(successSpy.at(0).at(0));
    QCOMPARE(animal.id, static_cast<qint64>(1));
    QCOMPARE(animal.name, QString("Updated Buddy"));
}

void TestAnimalService::testUpdateAnimal_NetworkError_EmitsUpdateAnimalFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::updateAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::updateAnimalFailed);

    AnimalUpdateDTO dto;
    dto.name = "Updated Buddy";

    service.updateAnimal(1, dto);
    mock.triggerError(mock.putCalls, serverErrorJson("Forbidden"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testUpdateAnimal_InvalidJson_EmitsUpdateAnimalFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::updateAnimalSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::updateAnimalFailed);

    AnimalUpdateDTO dto;
    dto.name = "Updated Buddy";

    service.updateAnimal(1, dto);
    mock.triggerSuccess(mock.putCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testUpdateAnimal_ServerError_DoesNotEmitOtherSignals() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy getAnimalFailed(&service, &AnimalService::getAnimalFailed);
    QSignalSpy createFailed(&service, &AnimalService::createAnimalFailed);
    QSignalSpy updateFailed(&service, &AnimalService::updateAnimalFailed);

    AnimalUpdateDTO dto;
    dto.name = "Updated Buddy";

    service.updateAnimal(1, dto);
    mock.triggerError(mock.putCalls, serverErrorJson());

    QCOMPARE(updateFailed.count(), 1);
    QCOMPARE(getAnimalFailed.count(), 0);
    QCOMPARE(createFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// getAnimalFilters signal tests

void TestAnimalService::testGetAnimalFilters_Success_EmitsGetAnimalFiltersSuccess() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalFiltersSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalFiltersFailed);

    service.getAnimalFilters();
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validFilterJson());

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto filter = qvariant_cast<AnimalFilterDTO>(successSpy.at(0).at(0));
    QVERIFY(filter.breeds.has_value());
    QCOMPARE(filter.breeds->size(), 2);
    QVERIFY(filter.sizes.has_value());
    QCOMPARE(filter.sizes->size(), 2);
    QVERIFY(filter.ageLte.has_value());
    QCOMPARE(*filter.ageLte, 5);
}

void TestAnimalService::testGetAnimalFilters_NetworkError_EmitsGetAnimalFiltersFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalFiltersSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalFiltersFailed);

    service.getAnimalFilters();
    mock.triggerError(mock.getCalls, serverErrorJson("Server error"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimalFilters_InvalidJson_EmitsGetAnimalFiltersFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalFiltersSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalFiltersFailed);

    service.getAnimalFilters();
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimalFilters_ServerError_DoesNotEmitOtherSignals() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy getAnimalsFailed(&service, &AnimalService::getAnimalsFailed);
    QSignalSpy createFailed(&service, &AnimalService::createAnimalFailed);
    QSignalSpy filtersFailed(&service, &AnimalService::getAnimalFiltersFailed);

    service.getAnimalFilters();
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(filtersFailed.count(), 1);
    QCOMPARE(getAnimalsFailed.count(), 0);
    QCOMPARE(createFailed.count(), 0);
}

// ---------------------------------------------------------------------------
// getAnimalsByOrganization signal tests

void TestAnimalService::testGetAnimalsByOrganization_Success_EmitsGetAnimalsByOrganizationSuccess() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalsByOrganizationSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalsByOrganizationFailed);

    service.getAnimalsByOrganization(10);
    QCOMPARE(mock.getCalls.size(), 1);

    mock.triggerSuccess(mock.getCalls, validAnimalArrayJson());

    QCOMPARE(successSpy.count(), 1);
    QCOMPARE(failedSpy.count(), 0);

    auto animals = qvariant_cast<QList<AnimalDTO>>(successSpy.at(0).at(0));
    QCOMPARE(animals.size(), 2);
    QCOMPARE(animals[0].name, QString("Buddy"));
    QCOMPARE(animals[1].name, QString("Whiskers"));
}

void TestAnimalService::testGetAnimalsByOrganization_NetworkError_EmitsGetAnimalsByOrganizationFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalsByOrganizationSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalsByOrganizationFailed);

    service.getAnimalsByOrganization(10);
    mock.triggerError(mock.getCalls, serverErrorJson("Not found"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimalsByOrganization_InvalidJson_EmitsGetAnimalsByOrganizationFailed() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy successSpy(&service, &AnimalService::getAnimalsByOrganizationSuccess);
    QSignalSpy failedSpy(&service, &AnimalService::getAnimalsByOrganizationFailed);

    service.getAnimalsByOrganization(10);
    mock.triggerSuccess(mock.getCalls, QByteArray("not valid json {{{}"));

    QCOMPARE(successSpy.count(), 0);
    QCOMPARE(failedSpy.count(), 1);
}

void TestAnimalService::testGetAnimalsByOrganization_ServerError_DoesNotEmitOtherSignals() {
    MockNetworkClient mock;
    AnimalService service(mock);

    QSignalSpy getAnimalsFailed(&service, &AnimalService::getAnimalsFailed);
    QSignalSpy getAnimalFailed(&service, &AnimalService::getAnimalFailed);
    QSignalSpy orgAnimalsFailed(&service, &AnimalService::getAnimalsByOrganizationFailed);

    service.getAnimalsByOrganization(10);
    mock.triggerError(mock.getCalls, serverErrorJson());

    QCOMPARE(orgAnimalsFailed.count(), 1);
    QCOMPARE(getAnimalsFailed.count(), 0);
    QCOMPARE(getAnimalFailed.count(), 0);
}

void TestAnimalService::testGetAnimalsByOrganization_UsesCorrectEndpoint() {
    MockNetworkClient mock;
    AnimalService service(mock);

    service.getAnimalsByOrganization(42);

    QCOMPARE(mock.getCalls.size(), 1);
    QVERIFY(mock.getCalls[0].endpoint.path().contains("/org/42/animals"));
}

QTEST_MAIN(TestAnimalService)

#include "animal_service_test.moc"
