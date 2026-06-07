#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QTemporaryFile>
#include <QtTest>

#include "services/errors.hpp"
#include "services/i_network_client.hpp"
#include "services/photo_service.hpp"

using namespace pawspective::services; // NOLINT google-build-using-namespace

// ---------------------------------------------------------------------------
// Fake QNetworkReply

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
// MockNetworkClient

class MockNetworkClient : public INetworkClient {
public:
    using CallbackHandler = INetworkClient::CallbackHandler;

    struct RawPostCall {
        QUrl endpoint;
        QByteArray body;
        QString contentType;
        CallbackHandler onSuccess;
        CallbackHandler onError;
    };

    struct GetCall {
        QUrl endpoint;
        CallbackHandler onSuccess;
        CallbackHandler onError;
    };

    QList<RawPostCall> rawPostCalls;
    QList<GetCall> getCalls;

    void get(const QUrl& url, CallbackHandler ok, CallbackHandler err) override {
        getCalls.append({url, ok, err});
    }
    void post(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void put(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void patch(const QUrl&, const QByteArray&, CallbackHandler, CallbackHandler) override {}
    void deleteResource(const QUrl&, CallbackHandler, CallbackHandler) override {}
    void postRaw(
        const QUrl& url,
        const QByteArray& data,
        const QString& contentType,
        CallbackHandler ok,
        CallbackHandler err
    ) override {
        rawPostCalls.append({url, data, contentType, ok, err});
    }

    void triggerRawPostSuccess(const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        rawPostCalls[idx].onSuccess(reply);
    }
    void triggerRawPostError(const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        rawPostCalls[idx].onError(reply);
    }
    void triggerGetSuccess(const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        getCalls[idx].onSuccess(reply);
    }
    void triggerGetError(const QByteArray& data, int idx = 0) {
        FakeNetworkReply reply(data);
        getCalls[idx].onError(reply);
    }
};

// ---------------------------------------------------------------------------

static QByteArray makePngBytes() {
    QImage img(4, 4, QImage::Format_RGB32);
    img.fill(Qt::red);
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "PNG");
    return bytes;
}

static QByteArray uploadSuccessJson(const QString& url = "https://storage.yandexcloud.net/bucket/photos/abc.jpg") {
    QJsonObject obj;
    obj["url"] = url;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

static QByteArray errorJson(const QString& message = "Upload failed") {
    QJsonObject obj;
    obj["message"] = message;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

// ---------------------------------------------------------------------------

class TestPhotoService : public QObject {
    Q_OBJECT

private slots:
    void testUploadPhoto_Success();
    void testUploadPhoto_InvalidFilePath();
    void testUploadPhoto_ServerError();
    void testUploadPhoto_MalformedResponse();
    void testGetPhoto_Success();
    void testGetPhoto_Error();
};

void TestPhotoService::testUploadPhoto_Success() {
    QTemporaryFile tmp;
    tmp.setFileTemplate(QDir::tempPath() + "/test_photo_XXXXXX.jpg");
    QVERIFY(tmp.open());
    tmp.write(makePngBytes());
    tmp.flush();
    tmp.close();

    MockNetworkClient mock;
    PhotoService service(mock);

    QString receivedUrl;
    connect(&service, &PhotoService::uploadPhotoSuccess, [&](const QString& url) { receivedUrl = url; });

    service.uploadPhoto(tmp.fileName());

    QCOMPARE(mock.rawPostCalls.size(), 1);
    QCOMPARE(mock.rawPostCalls[0].endpoint, QUrl("/upload/photo"));
    QVERIFY(!mock.rawPostCalls[0].body.isEmpty());

    const QString expectedUrl = "https://storage.yandexcloud.net/bucket/photos/abc.jpg";
    mock.triggerRawPostSuccess(uploadSuccessJson(expectedUrl));

    QCOMPARE(receivedUrl, expectedUrl);
}

void TestPhotoService::testUploadPhoto_InvalidFilePath() {
    MockNetworkClient mock;
    PhotoService service(mock);

    QSharedPointer<BaseError> receivedError;
    connect(&service, &PhotoService::uploadPhotoFailed, [&](QSharedPointer<BaseError> err) {
        receivedError = err;
    });

    service.uploadPhoto("/nonexistent/path/to/file.jpg");

    QVERIFY(mock.rawPostCalls.isEmpty());
    QVERIFY(!receivedError.isNull());
}

void TestPhotoService::testUploadPhoto_ServerError() {
    QTemporaryFile tmp;
    tmp.setFileTemplate(QDir::tempPath() + "/test_photo_XXXXXX.png");
    QVERIFY(tmp.open());
    tmp.write(makePngBytes());
    tmp.flush();
    tmp.close();

    MockNetworkClient mock;
    PhotoService service(mock);

    QSharedPointer<BaseError> receivedError;
    connect(&service, &PhotoService::uploadPhotoFailed, [&](QSharedPointer<BaseError> err) {
        receivedError = err;
    });

    service.uploadPhoto(tmp.fileName());
    QCOMPARE(mock.rawPostCalls.size(), 1);

    mock.triggerRawPostError(errorJson("Upload failed"));

    QVERIFY(!receivedError.isNull());
}

void TestPhotoService::testUploadPhoto_MalformedResponse() {
    QTemporaryFile tmp;
    tmp.setFileTemplate(QDir::tempPath() + "/test_photo_XXXXXX.png");
    QVERIFY(tmp.open());
    tmp.write(makePngBytes());
    tmp.flush();
    tmp.close();

    MockNetworkClient mock;
    PhotoService service(mock);

    QSharedPointer<BaseError> receivedError;
    connect(&service, &PhotoService::uploadPhotoFailed, [&](QSharedPointer<BaseError> err) {
        receivedError = err;
    });

    service.uploadPhoto(tmp.fileName());
    mock.triggerRawPostSuccess(QByteArray("not valid json {{{"));

    QVERIFY(!receivedError.isNull());
    QVERIFY(receivedError.dynamicCast<ClientJsonParseError>());
}

void TestPhotoService::testGetPhoto_Success() {
    MockNetworkClient mock;
    PhotoService service(mock);

    QImage receivedImage;
    connect(&service, &PhotoService::getPhotoSuccess, [&](const QImage& img) { receivedImage = img; });

    const QString url = "https://storage.yandexcloud.net/bucket/photos/abc.png";
    service.getPhoto(url);

    QCOMPARE(mock.getCalls.size(), 1);
    QCOMPARE(mock.getCalls[0].endpoint, QUrl(url));

    mock.triggerGetSuccess(makePngBytes());

    QVERIFY(!receivedImage.isNull());
    QCOMPARE(receivedImage.width(), 4);
    QCOMPARE(receivedImage.height(), 4);
}

void TestPhotoService::testGetPhoto_Error() {
    MockNetworkClient mock;
    PhotoService service(mock);

    QSharedPointer<BaseError> receivedError;
    connect(&service, &PhotoService::getPhotoFailed, [&](QSharedPointer<BaseError> err) {
        receivedError = err;
    });

    service.getPhoto("https://storage.yandexcloud.net/bucket/photos/abc.png");
    mock.triggerGetError(QByteArray("Not found"));

    QVERIFY(!receivedError.isNull());
}

QTEST_MAIN(TestPhotoService)
#include "photo_service_test.moc"
