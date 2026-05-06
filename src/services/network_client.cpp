#include "services/network_client.hpp"

#include <QNetworkCookieJar>
#include <QNetworkReply>
#include "utils/logger.hpp"
#include "services/errors.hpp"

namespace pawspective::services {

NetworkClient::NetworkClient(QObject* parent) : QObject(parent) { 
    DEBUG_LOG("NetworkClient", "Constructor called");
    m_manager.setCookieJar(new QNetworkCookieJar(this)); 
}

void NetworkClient::sendRequest(
    HttpMethod method,
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    const char* methodStr = "UNKNOWN";
    switch(method) {
        case HttpMethod::Get: methodStr = "GET"; break;
        case HttpMethod::Post: methodStr = "POST"; break;
        case HttpMethod::Put: methodStr = "PUT"; break;
        case HttpMethod::Patch: methodStr = "PATCH"; break;
        case HttpMethod::Delete: methodStr = "DELETE"; break;
    }
    DEBUG_LOG("NetworkClient::sendRequest", "Starting" << methodStr << "request to:" << endpoint.toString());
    DEBUG_LOG("NetworkClient::sendRequest", "Data size:" << data.size() << "bytes");
    
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
        DEBUG_LOG("NetworkClient::sendRequest", "ERROR: reply is null!");
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
            QByteArray responseData = reply->readAll();
            reply->setProperty("responseData", responseData);
            
            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            DEBUG_LOG("NetworkClient::sendRequest::finished", "HTTP Status:" << httpStatus);
            DEBUG_LOG("NetworkClient::sendRequest::finished", "Reply error:" << reply->error());
            DEBUG_LOG("NetworkClient::sendRequest::finished", "Response size:" << responseData.size() << "bytes");
            
            if (reply->error() != QNetworkReply::NoError) {
                DEBUG_LOG("NetworkClient::sendRequest::finished", "Network error:" << reply->errorString());
                if (responseData.isEmpty()) {
                    reply->setProperty("responseData", QByteArray("Network error: ") + reply->errorString().toUtf8());
                }

                if (onError) {
                    qDebug() << "[NetworkClient::sendRequest::finished] Calling onError callback";
                    onError(*reply);
                }

                reply->deleteLater();
                return;
            }
            if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
                DEBUG_LOG("NetworkClient::sendRequest::finished", "Got 401 Unauthorized");
                QSharedPointer<BaseError> error = ErrorFactory::createError(responseData);
                if (error.dynamicCast<AccessTokenExpiredError>()) {
                    DEBUG_LOG("NetworkClient::sendRequest::finished", "Token expired, queuing request");
                    m_pendingRequests.append({method, endpoint, data, onSuccess, onError});

                    if (!m_isRefreshing) {
                        m_isRefreshing = true;
                        emit unauthorizedAccess();
                    }
                    reply->deleteLater();
                    return;
                }
                if (error.dynamicCast<AccessTokenInvalidError>()) {
                    DEBUG_LOG("NetworkClient::sendRequest::finished", "Token invalid");
                    emit invalidTokenDetected();
                    reply->deleteLater();
                    return;
                }
                reply->setProperty("responseData", responseData);
                if (onError) {
                    DEBUG_LOG("NetworkClient::sendRequest::finished", "Calling onError callback (401 other)");
                    onError(*reply);
                }
                reply->deleteLater();
                return;
            }
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            if (statusCode >= 200 && statusCode < 300) {
                DEBUG_LOG("NetworkClient::sendRequest::finished", "Success (2xx status)");
                if (onSuccess) {
                    DEBUG_LOG("NetworkClient::sendRequest::finished", "Calling onSuccess callback");
                    onSuccess(*reply);
                }
            } else {
                DEBUG_LOG("NetworkClient::sendRequest::finished", "Error status code:" << statusCode);
                if (onError) {
                    DEBUG_LOG("NetworkClient::sendRequest::finished", "Calling onError callback");
                    onError(*reply);
                }
            }
            reply->deleteLater();
        }
    );
}

void NetworkClient::retryPendingRequests() {
    DEBUG_LOG("NetworkClient::retryPendingRequests", "Retrying" << m_pendingRequests.size() << "pending requests");
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
    DEBUG_LOG("NetworkClient::clearPendingRequests", "Clearing" << m_pendingRequests.size() << "pending requests");
    m_isRefreshing = false;
    m_pendingRequests.clear();
}

void NetworkClient::get(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) {
    DEBUG_LOG("NetworkClient::get", "Called for endpoint:" << endpoint.toString());
    sendRequest(HttpMethod::Get, endpoint, {}, std::move(onSuccess), std::move(onError));
}

void NetworkClient::post(
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    qDebug() << "[NetworkClient::post] Called for endpoint:" << endpoint.toString();
    sendRequest(HttpMethod::Post, endpoint, data, std::move(onSuccess), std::move(onError));
}

void NetworkClient::put(
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    qDebug() << "[NetworkClient::put] Called for endpoint:" << endpoint.toString();
    sendRequest(HttpMethod::Put, endpoint, data, std::move(onSuccess), std::move(onError));
}

void NetworkClient::patch(
    const QUrl& endpoint,
    const QByteArray& data,
    CallbackHandler onSuccess,
    CallbackHandler onError
) {
    qDebug() << "[NetworkClient::patch] Called for endpoint:" << endpoint.toString();
    sendRequest(HttpMethod::Patch, endpoint, data, std::move(onSuccess), std::move(onError));
}

void NetworkClient::deleteResource(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) {
    qDebug() << "[NetworkClient::deleteResource] Called for endpoint:" << endpoint.toString();
    sendRequest(HttpMethod::Delete, endpoint, {}, std::move(onSuccess), std::move(onError));
}

QNetworkRequest NetworkClient::createRequest(const QUrl& endpoint) const {
    qDebug() << "[NetworkClient::createRequest] Creating request for:" << endpoint.toString();
    QNetworkRequest request(m_baseUrl.resolved(endpoint));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Pawspective/1.0");
    if (m_tokenProvider) {
        QString token = m_tokenProvider();
        if (!token.isEmpty()) {
            qDebug() << "[NetworkClient::createRequest] Token provider returned token of length:" << token.length();
            request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        } else {
            qDebug() << "[NetworkClient::createRequest] Token provider returned empty token";
        }
    } else {
        qDebug() << "[NetworkClient::createRequest] No token provider set";
    }
    request.setTransferTimeout(m_timeout);
    return request;
}

void NetworkClient::setTokenProvider(TokenProvider provider) { 
    qDebug() << "[NetworkClient::setTokenProvider] Called";
    m_tokenProvider = std::move(provider); 
}

void NetworkClient::setUserId(std::optional<uint64_t> userId) { 
    if (userId.has_value()) {
        qDebug() << "[NetworkClient::setUserId] Setting user ID to:" << userId.value();
    } else {
        qDebug() << "[NetworkClient::setUserId] Clearing user ID";
    }
    m_userId = userId; 
}

std::optional<uint64_t> NetworkClient::getUserId() const { return m_userId; }

}  // namespace pawspective::services