#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "mainwindow.hpp"
#include "services/auth_service.hpp"
#include "services/city_service.hpp"
#include "services/organization_service.hpp"
#include "services/user_service.hpp"
#include "viewmodels/login_view_model.hpp"
#include "viewmodels/register_organization_view_model.hpp"
#include "viewmodels/register_view_model.hpp"

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
    pawspective::services::UserService userService(networkClient);
    pawspective::services::OrganizationService organizationService(networkClient);
    pawspective::services::CityService cityService(networkClient);
    auto loginViewModel = new pawspective::viewmodels::LoginViewModel(authService, &app);
    auto registerViewModel = new pawspective::viewmodels::RegisterViewModel(userService, &app);
    auto registerOrganizationViewModel =
        new pawspective::viewmodels::RegisterOrganizationViewModel(organizationService, cityService, &app);
    engine.rootContext()->setContextProperty("loginViewModel", loginViewModel);
    engine.rootContext()->setContextProperty("registerViewModel", registerViewModel);
    engine.rootContext()->setContextProperty("registerOrganizationViewModel", registerOrganizationViewModel);

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
