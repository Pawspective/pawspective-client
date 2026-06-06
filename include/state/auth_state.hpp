#pragma once
#include <QObject>
#include <QString>

namespace pawspective::state {

class AuthState : public QObject {
    Q_OBJECT
public:
    explicit AuthState(QObject* parent = nullptr);

    void saveRefreshToken(const QString& token);
    void clearRefreshToken();
    void loadRefreshToken();

signals:
    void refreshTokenLoaded(const QString& token);
    void refreshTokenLoadFailed();
};

}  // namespace pawspective::state
