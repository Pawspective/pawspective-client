#include "services/network_client.hpp"

#include <QNetworkCookieJar>
#include <QNetworkReply>

namespace pawspective::services {

NetworkClient::NetworkClient(QObject* parent) : QObject(parent) { m_manager.setCookieJar(new QNetworkCookieJar(this)); }

void NetworkClient::sendRequest(
    HttpMethod method,
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply* reply = nullptr;

    switch (method) {
        case HttpMethod::Get:
            reply = m_manager.get(request);
            break;
        case HttpMethod::Post:
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            reply = m_manager.post(request, data);
            break;
        case HttpMethod::Put:
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            reply = m_manager.put(request, data);
            break;
        case HttpMethod::Patch:
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            reply = m_manager.sendCustomRequest(request, "PATCH", data);
            break;
        case HttpMethod::Delete:
            reply = m_manager.deleteResource(request);
            break;
    }

    if (!reply) {
        return;
    }

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [this,
         method,
         endpoint = request.url(),
         data,
         reply,
         onSuccess = std::move(onSuccess),
         onError = std::move(onError)]() {
            if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
                m_pendingRequests.append({method, endpoint, data, onSuccess, onError});

                if (!m_isRefreshing) {
                    m_isRefreshing = true;
                    emit unauthorizedAccess();
                }
                reply->deleteLater();
                return;
            }
            try {
                if (reply->error() == QNetworkReply::NoError) {
                    if (onSuccess) {
                        onSuccess(*reply);
                    }
                } else {
                    if (onError) {
                        onError(*reply);
                    }
                }
            } catch (const std::exception& e) {
                qWarning() << "Exception in network reply handler:" << e.what();
            }
            reply->deleteLater();
        }
    );
}

void NetworkClient::retryPendingRequests() {
    m_isRefreshing = false;
    QList<PendingRequest> pendingRequests = std::move(m_pendingRequests);
    m_pendingRequests.clear();
    for (auto& request : pendingRequests) {
        sendRequest(
            request.method,
            request.endpoint,
            request.data,
            std::move(request.onSuccess),
            std::move(request.onError)
        );
    }
}

void NetworkClient::clearPendingRequests() {
    m_isRefreshing = false;
    m_pendingRequests.clear();
}

void NetworkClient::get(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) {
    sendRequest(HttpMethod::Get, endpoint, {}, std::move(onSuccess), std::move(onError));
}

void NetworkClient::post(
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    sendRequest(HttpMethod::Post, endpoint, data, std::move(onSuccess), std::move(onError));
}

void NetworkClient::put(
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    sendRequest(HttpMethod::Put, endpoint, data, std::move(onSuccess), std::move(onError));
}

void NetworkClient::patch(
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    sendRequest(HttpMethod::Patch, endpoint, data, std::move(onSuccess), std::move(onError));
}

void NetworkClient::deleteResource(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) {
    sendRequest(HttpMethod::Delete, endpoint, {}, std::move(onSuccess), std::move(onError));
}

QNetworkRequest NetworkClient::createRequest(const QUrl& endpoint) const {
    QNetworkRequest request(m_baseUrl.resolved(endpoint));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Pawspective/1.0");
    if (m_tokenProvider) {
        QString token = m_tokenProvider();
        if (!token.isEmpty()) {
            request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        }
    }
    request.setTransferTimeout(m_timeout);
    return request;
}

void NetworkClient::setTokenProvider(TokenProvider provider) { m_tokenProvider = std::move(provider); }

void NetworkClient::setUserId(std::optional<uint64_t> userId) { m_userId = userId; }

std::optional<uint64_t> NetworkClient::getUserId() const { return m_userId; }

}  // namespace pawspective::services