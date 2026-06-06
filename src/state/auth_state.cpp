#include "state/auth_state.hpp"
#include <keychain.h>
#include <QDebug>
#include <QObject>

namespace pawspective::state {

namespace {
constexpr auto kService = "pawspective";
constexpr auto kKey = "auth/refresh_token";
}  // namespace

AuthState::AuthState(QObject* parent) : QObject(parent) {}

void AuthState::saveRefreshToken(const QString& token) {
    auto* job = new QKeychain::WritePasswordJob(kService, this);
    job->setKey(kKey);
    job->setTextData(token);
    connect(job, &QKeychain::Job::finished, this, [](QKeychain::Job* j) {
        if (j->error() != QKeychain::NoError) {
            qWarning() << "Keychain write failed:" << j->errorString();
        }
    });
    job->start();
}

void AuthState::clearRefreshToken() {
    auto* job = new QKeychain::DeletePasswordJob(kService, this);
    job->setKey(kKey);
    connect(job, &QKeychain::Job::finished, this, [](QKeychain::Job* j) {
        if (j->error() != QKeychain::NoError && j->error() != QKeychain::EntryNotFound) {
            qWarning() << "Keychain delete failed:" << j->errorString();
        }
    });
    job->start();
}

void AuthState::loadRefreshToken() {
    auto* job = new QKeychain::ReadPasswordJob(kService, this);
    job->setKey(kKey);
    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job* finishedJob) {
        if (finishedJob->error() == QKeychain::NoError) {
            emit refreshTokenLoaded(static_cast<QKeychain::ReadPasswordJob*>(finishedJob)->textData());
        } else {
            if (finishedJob->error() != QKeychain::EntryNotFound) {
                qWarning() << "Keychain read failed:" << finishedJob->errorString();
            }
            emit refreshTokenLoadFailed();
        }
    });
    job->start();
}

}  // namespace pawspective::state
