#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "services/auth_service.hpp"
#include "services/network_client.hpp"
#include "services/user_service.hpp"
#include "viewmodels/user_update_viewmodel.hpp"
#include "viewmodels/user_viewmodel.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    auto networkClient = std::make_unique<pawspective::services::NetworkClient>();
    auto authService = std::make_unique<pawspective::services::AuthService>(*networkClient);
    auto userService = std::make_unique<pawspective::services::UserService>(*networkClient);

    auto userViewModel = std::make_unique<pawspective::viewmodels::UserViewModel>(*authService, *userService);
    auto userUpdateViewModel = std::make_unique<
        pawspective::viewmodels::UserUpdateViewModel>(*userService, *authService);

    QQmlApplicationEngine engine;

    qmlRegisterType<pawspective::viewmodels::UserViewModel>("Pawspective.ViewModels", 1, 0, "UserViewModel");
    qmlRegisterType<
        pawspective::viewmodels::UserUpdateViewModel>("Pawspective.ViewModels", 1, 0, "UserUpdateViewModel");

    engine.rootContext()->setContextProperty("userViewModel", userViewModel.get());
    engine.rootContext()->setContextProperty("userUpdateViewModel", userUpdateViewModel.get());

    const QUrl url(QStringLiteral("qrc:/pawspective/qml/Main.qml"));

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

    return app.exec();
}