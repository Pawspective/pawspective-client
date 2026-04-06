#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "mainwindow.hpp"
#include "services/animal_service.hpp"
#include "services/auth_service.hpp"
#include "services/breed_service.hpp"
#include "services/city_service.hpp"
#include "services/organization_service.hpp"
#include "services/user_service.hpp"
#include "viewmodels/animal_detail_viewmodel.hpp"
#include "viewmodels/create_animal_viewmodel.hpp"
#include "viewmodels/login_view_model.hpp"
#include "viewmodels/organization_card_viewmodel.hpp"
#include "viewmodels/organization_view_model.hpp"
#include "viewmodels/register_organization_view_model.hpp"
#include "viewmodels/register_view_model.hpp"
#include "viewmodels/update_organization_viewmodel.hpp"
#include "viewmodels/user_update_viewmodel.hpp"
#include "viewmodels/user_viewmodel.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qRegisterMetaType<pawspective::models::UserDTO>("UserDTO");
    qRegisterMetaType<pawspective::models::UserDTO>("pawspective::models::UserDTO");

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
    pawspective::services::AnimalService animalService(networkClient);
    pawspective::services::BreedService breedService(networkClient);
    auto loginViewModel = new pawspective::viewmodels::LoginViewModel(authService, &app);
    auto registerViewModel = new pawspective::viewmodels::RegisterViewModel(userService, &app);
    auto registerOrganizationViewModel =
        new pawspective::viewmodels::RegisterOrganizationViewModel(organizationService, cityService, &app);
    auto organizationViewModel =
        new pawspective::viewmodels::OrganizationViewModel(authService, organizationService, &app);
    auto userViewModel = new pawspective::viewmodels::UserViewModel(authService, userService, &app);
    auto userUpdateViewModel = new pawspective::viewmodels::UserUpdateViewModel(userService, authService);
    auto updateOrganizationViewModel =
        new pawspective::viewmodels::UpdateOrganizationViewModel(organizationService, cityService, authService, &app);
    auto createAnimalViewModel = new pawspective::viewmodels::CreateAnimalViewModel(animalService, breedService, &app);
    auto organizationCardViewModel = new pawspective::viewmodels::OrganizationCardViewModel(&app);
    auto animalDetailViewModel =
        new pawspective::viewmodels::AnimalDetailViewModel(animalService, organizationService, &app);

    engine.rootContext()->setContextProperty("loginViewModel", loginViewModel);
    engine.rootContext()->setContextProperty("authService", &authService);
    engine.rootContext()->setContextProperty("registerViewModel", registerViewModel);
    engine.rootContext()->setContextProperty("registerOrganizationViewModel", registerOrganizationViewModel);
    engine.rootContext()->setContextProperty("organizationViewModel", organizationViewModel);
    engine.rootContext()->setContextProperty("userViewModel", userViewModel);
    engine.rootContext()->setContextProperty("updateOrganizationViewModel", updateOrganizationViewModel);
    engine.rootContext()->setContextProperty("userUpdateViewModel", userUpdateViewModel);
    engine.rootContext()->setContextProperty("createAnimalViewModel", createAnimalViewModel);
    engine.rootContext()->setContextProperty("organizationCardViewModel", organizationCardViewModel);
    engine.rootContext()->setContextProperty("animalDetailViewModel", animalDetailViewModel);

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