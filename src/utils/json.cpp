#include "utils/json.hpp"

#include <format>
#include <string_view>

namespace pawspective::utils::json {

QString getRequiredString(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull() || !json[qkey].isString()) {
        throw std::invalid_argument(std::format("Invalid or missing {} field", key));
    }
    return json[qkey].toString();
}

qint64 getRequiredInt64(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull() || !json[qkey].isDouble()) {
        throw std::invalid_argument(std::format("Invalid or missing {} field", key));
    }
    return json[qkey].toVariant().toLongLong();
}

qint32 getRequiredInt32(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull() || !json[qkey].isDouble()) {
        throw std::invalid_argument(std::format("Invalid or missing {} field", key));
    }
    return json[qkey].toInt();
}

QJsonObject getRequiredObject(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull() || !json[qkey].isObject()) {
        throw std::invalid_argument(std::format("Invalid or missing {} field", key));
    }
    return json[qkey].toObject();
}

std::optional<QString> getOptionalString(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull()) {
        return std::nullopt;
    }
    if (!json[qkey].isString()) {
        throw std::invalid_argument(std::format("Invalid {} field", key));
    }
    return json[qkey].toString();
}

std::optional<qint64> getOptionalInt64(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull()) {
        return std::nullopt;
    }
    if (!json[qkey].isDouble()) {
        throw std::invalid_argument(std::format("Invalid {} field", key));
    }
    return json[qkey].toVariant().toLongLong();
}

std::optional<qint32> getOptionalInt32(const QJsonObject& json, std::string_view key) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull()) {
        return std::nullopt;
    }
    if (!json[qkey].isDouble()) {
        throw std::invalid_argument(std::format("Invalid {} field", key));
    }
    return json[qkey].toInt();
}

}  // namespace pawspective::utils::json
