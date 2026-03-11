#pragma once

#include <QNetworkAccessManager>
#include <QQueue>
#include <functional>

#include "services/i_network_client.hpp"

namespace pawspective::services {

enum class HttpMethod : uint8_t { Get, Post, Put, Patch, Delete };

class NetworkClient final : public QObject, public INetworkClient {
    Q_OBJECT
public:
    using CallbackHandler = INetworkClient::CallbackHandler;
    using TokenProvider = std::function<QString()>;

    explicit NetworkClient(QObject* parent = nullptr);

    void get(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) override;
    void post(
        const QUrl& endpoint,
        const QByteArray& data,
        CallbackHandler onSuccess,
        CallbackHandler onError
    ) override;
    void put(const QUrl& endpoint, const QByteArray& data, CallbackHandler onSuccess, CallbackHandler onError) override;
    void patch(
        const QUrl& endpoint,
        const QByteArray& data,
        CallbackHandler onSuccess,
        CallbackHandler onError
    ) override;
    void deleteResource(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) override;

    void setTokenProvider(TokenProvider provider);
    void setUserId(std::optional<uint64_t> userId);
    std::optional<uint64_t> getUserId() const;

signals:
    void unauthorizedAccess();

public slots:
    void retryPendingRequests();
    void clearPendingRequests();

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
    std::optional<uint64_t> m_userId;
    bool m_isRefreshing = false;
    TokenProvider m_tokenProvider;

    const QUrl m_baseUrl = QUrl("http://localhost:8080/");
};

}  // namespace pawspective::services
