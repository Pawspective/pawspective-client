#pragma once

#include <QNetworkAccessManager>
#include <QQueue>
#include <functional>

namespace pawspective::services {

enum class HttpMethod : uint8_t { Get, Post, Put, Patch, Delete };

class NetworkClient final : public QObject {
    Q_OBJECT
public:
    using CallbackHandler = std::function<void(const QNetworkReply&)>;
    using TokenProvider = std::function<QString()>;

    explicit NetworkClient(QObject* parent = nullptr);

    void get(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError);
    void post(const QUrl& endpoint, const QByteArray& data, CallbackHandler onSuccess, CallbackHandler onError);
    void put(const QUrl& endpoint, const QByteArray& data, CallbackHandler onSuccess, CallbackHandler onError);
    void patch(const QUrl& endpoint, const QByteArray& data, CallbackHandler onSuccess, CallbackHandler onError);
    void deleteResource(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError);

    void setTokenProvider(TokenProvider provider);

signals:
    void unauthorizedAccess();

public slots:
    void retryPendingRequests();

private:
    struct PendingRequest {
        HttpMethod method;
        QUrl endpoint;
        QByteArray data;
        CallbackHandler onSuccess;
        CallbackHandler onError;
    };

    void sendRequest(
        HttpMethod method,
        const QUrl& endpoint,
        const QByteArray& data,
        CallbackHandler onSuccess,
        CallbackHandler onError
    );
    QNetworkRequest createRequest(const QUrl& endpoint) const;

    QNetworkAccessManager m_manager;
    int m_timeout = 5000;
    QList<PendingRequest> m_pendingRequests;
    bool m_isRefreshing = false;
    TokenProvider m_tokenProvider;

    const QUrl m_baseUrl = QUrl("http://localhost:8080/");
};

}  // namespace pawspective::services
