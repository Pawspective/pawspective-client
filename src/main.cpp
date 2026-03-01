#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "mainwindow.hpp"
#include "services/auth_service.hpp"
#include "viewmodels/login_view_model.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/pawspective/qml/Main.qml"));
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        qDebug() << "Resource file:" << it.next();
    }

    pawspective::services::NetworkClient networkClient(&app);
    pawspective::services::AuthService authService(networkClient);
    auto loginViewModel = new pawspective::viewmodels::LoginViewModel(authService, &app);

    engine.rootContext()->setContextProperty("loginViewModel", loginViewModel);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](const QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection
    );

    engine.load(url);

    return app.exec();  // NOLINT
}
