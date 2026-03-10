#pragma once

#include <QByteArray>
#include <QUrl>
#include <functional>

class QNetworkReply;

namespace pawspective::services {

class INetworkClient {
public:
    using CallbackHandler = std::function<void(QNetworkReply&)>;

    virtual ~INetworkClient() = default;

    virtual void get(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) = 0;
    virtual void post(
        const QUrl& endpoint,
        const QByteArray& data,
        CallbackHandler onSuccess,
        CallbackHandler onError
    ) = 0;
    virtual void put(
        const QUrl& endpoint,
        const QByteArray& data,
        CallbackHandler onSuccess,
        CallbackHandler onError
    ) = 0;
    virtual void patch(
        const QUrl& endpoint,
        const QByteArray& data,
        CallbackHandler onSuccess,
        CallbackHandler onError
    ) = 0;
    virtual void deleteResource(const QUrl& endpoint, CallbackHandler onSuccess, CallbackHandler onError) = 0;
};

}  // namespace pawspective::services
