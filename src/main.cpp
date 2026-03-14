#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "mainwindow.hpp"
#include "services/auth_service.hpp"
#include "services/user_service.hpp"
#include "viewmodels/login_view_model.hpp"
#include "viewmodels/register_view_model.hpp"
#include "viewmodels/user_update_viewmodel.hpp"
#include "viewmodels/user_viewmodel.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qRegisterMetaType<pawspective::models::UserDTO>("UserDTO");
    qRegisterMetaType<pawspective::models::UserDTO>("pawspective::models::UserDTO");

    pawspective::services::NetworkClient networkClient(&app);
    pawspective::services::AuthService authService(networkClient);
    pawspective::services::UserService userService(networkClient);

    auto userViewModel = new pawspective::viewmodels::UserViewModel(authService, userService, &app);
    auto userUpdateViewModel = new pawspective::viewmodels::UserUpdateViewModel(userService, authService);
    auto loginViewModel = new pawspective::viewmodels::LoginViewModel(authService, &app);
    auto registerViewModel = new pawspective::viewmodels::RegisterViewModel(userService, &app);

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/pawspective/qml/Main.qml"));
    engine.rootContext()->setContextProperty("userViewModel", userViewModel);
    engine.rootContext()->setContextProperty("userUpdateViewModel", userUpdateViewModel);

    engine.rootContext()->setContextProperty("loginViewModel", loginViewModel);
    engine.rootContext()->setContextProperty("registerViewModel", registerViewModel);

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