#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <string_view>

namespace pawspective::utils::json {

QString getRequiredString(const QJsonObject& json, std::string_view key);
qint64 getRequiredInt64(const QJsonObject& json, std::string_view key);
qint32 getRequiredInt32(const QJsonObject& json, std::string_view key);
QJsonObject getRequiredObject(const QJsonObject& json, std::string_view key);

std::optional<QString> getOptionalString(const QJsonObject& json, std::string_view key);
std::optional<qint64> getOptionalInt64(const QJsonObject& json, std::string_view key);
std::optional<qint32> getOptionalInt32(const QJsonObject& json, std::string_view key);

template <typename EnumType, typename FromApiFunc>
std::optional<EnumType> getOptionalEnum(const QJsonObject& json, std::string_view key, FromApiFunc fromApi) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey) || json[qkey].isNull()) {
        return std::nullopt;
    }
    if (json[qkey].isString()) {
        return fromApi(json[qkey].toString());
    }
    throw std::invalid_argument(std::format("Invalid {} field", key));
}

template <typename EnumType, typename FromApiFunc>
std::optional<QVector<EnumType>> getOptionalEnumArray(
    const QJsonObject& json,
    std::string_view key,
    FromApiFunc fromApi
) {
    QString qkey = QString::fromUtf8(key.data(), key.size());
    if (!json.contains(qkey)) {
        return std::nullopt;
    }
    if (!json[qkey].isArray()) {
        throw std::invalid_argument(std::format("Invalid {} field: expected array", key));
    }
    QJsonArray array = json[qkey].toArray();
    QVector<EnumType> result;
    for (const auto& item : array) {
        if (item.isString()) {
            result.append(fromApi(item.toString()));
        } else {
            throw std::invalid_argument("Invalid value in enum array");
        }
    }
    return result;
}

}  // namespace pawspective::utils::json
