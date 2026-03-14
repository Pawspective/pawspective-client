#pragma once

#include <QJsonObject>
#include <QString>
#include <optional>

namespace pawspective::models {

struct UserDTO {
    Q_GADGET

    Q_PROPERTY(QString email MEMBER email)
    Q_PROPERTY(QString firstName MEMBER firstName)
    Q_PROPERTY(QString lastName MEMBER lastName)

public:
    qint64 id = 0;
    QString email;
    QString firstName;
    QString lastName;
    std::optional<qint64> organizationId;

    bool operator==(const UserDTO& other) const {
        return id == other.id && email == other.email && firstName == other.firstName && lastName == other.lastName;
    }

    QJsonObject toJson() const;
    static UserDTO fromJson(const QJsonObject& json);

    static QString readString(const QJsonObject& json, const QString& key);
};

}  // namespace pawspective::models

Q_DECLARE_METATYPE(pawspective::models::UserDTO)