#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

#include "mainwindow.hpp"
#include "services/adopt_request_service.hpp"
#include "services/animal_service.hpp"
#include "services/auth_service.hpp"
#include "services/breed_service.hpp"
#include "services/city_service.hpp"
#include "services/organization_service.hpp"
#include "services/photo_service.hpp"
#include "services/post_service.hpp"
#include "services/review_service.hpp"
#include "services/user_service.hpp"
#include "state/auth_state.hpp"
#include "viewmodels/adopt_request_list_viewmodel.hpp"
#include "viewmodels/animal_detail_viewmodel.hpp"
#include "viewmodels/animal_list_viewmodel.hpp"
#include "viewmodels/create_animal_viewmodel.hpp"
#include "viewmodels/create_post_viewmodel.hpp"
#include "viewmodels/create_review_viewmodel.hpp"
#include "viewmodels/login_view_model.hpp"
#include "viewmodels/organization_card_viewmodel.hpp"
#include "viewmodels/organization_view_model.hpp"
#include "viewmodels/photo_upload_viewmodel.hpp"
#include "viewmodels/post_card_viewmodel.hpp"
#include "viewmodels/post_list_viewmodel.hpp"
#include "viewmodels/register_organization_view_model.hpp"
#include "viewmodels/register_view_model.hpp"
#include "viewmodels/review_list_viewmodel.hpp"
#include "viewmodels/search_organization_viewmodel.hpp"
#include "viewmodels/update_animal_viewmodel.hpp"
#include "viewmodels/update_organization_viewmodel.hpp"
#include "viewmodels/update_post_viewmodel.hpp"
#include "viewmodels/update_review_viewmodel.hpp"
#include "viewmodels/user_update_viewmodel.hpp"
#include "viewmodels/user_viewmodel.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qRegisterMetaType<pawspective::models::UserDTO>("UserDTO");
    qRegisterMetaType<pawspective::models::UserDTO>("pawspective::models::UserDTO");

    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/pawspective/qml/Main.qml"));
    QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //     qDebug() << "Resource file:" << it.next();
    // }

    pawspective::services::NetworkClient networkClient(&app);
    pawspective::state::AuthState authState(&app);
    pawspective::services::AuthService authService(networkClient, authState);
    pawspective::services::UserService userService(networkClient);
    pawspective::services::OrganizationService organizationService(networkClient);
    pawspective::services::CityService cityService(networkClient);
    pawspective::services::AnimalService animalService(networkClient);
    pawspective::services::BreedService breedService(networkClient);
    pawspective::services::AdoptRequestService adoptRequestService(networkClient);
    pawspective::services::PostService postService(networkClient);
    pawspective::services::ReviewService reviewService(networkClient);
    pawspective::services::PhotoService photoService(networkClient);
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
    auto createReviewViewModel = new pawspective::viewmodels::CreateReviewViewModel(reviewService, animalService, &app);
    auto organizationCardViewModel = new pawspective::viewmodels::OrganizationCardViewModel(&app);
    auto animalDetailViewModel = new pawspective::viewmodels::AnimalDetailViewModel(
        animalService,
        organizationService,
        adoptRequestService,
        &app
    );
    auto searchOrganizationViewModel =
        new pawspective::viewmodels::SearchOrganizationViewModel(organizationService, &app);
    auto updateAnimalViewModel = new pawspective::viewmodels::UpdateAnimalViewModel(animalService, breedService, &app);
    auto animalListViewModel = new pawspective::viewmodels::AnimalListViewModel(
        animalService,
        breedService,
        organizationService,
        cityService,
        &app
    );
    auto createPostViewModel = new pawspective::viewmodels::CreatePostViewModel(postService, &app);
    auto reviewListViewModel = new pawspective::viewmodels::ReviewListViewModel(reviewService, &app);
    auto postListViewModel = new pawspective::viewmodels::PostListViewModel(postService, &app);
    auto postCardViewModel = new pawspective::viewmodels::PostCardViewModel(&app);
    auto adoptRequestListViewModel = new pawspective::viewmodels::AdoptRequestListViewModel(adoptRequestService, &app);
    auto updatePostViewModel = new pawspective::viewmodels::UpdatePostViewModel(postService, &app);
    auto updateReviewViewModel = new pawspective::viewmodels::UpdateReviewViewModel(reviewService, &app);
    auto photoUploadViewModel = new pawspective::viewmodels::PhotoUploadViewModel(photoService, &app);

    engine.rootContext()->setContextProperty("loginViewModel", loginViewModel);
    engine.rootContext()->setContextProperty("authService", &authService);
    engine.rootContext()->setContextProperty("registerViewModel", registerViewModel);
    engine.rootContext()->setContextProperty("registerOrganizationViewModel", registerOrganizationViewModel);
    engine.rootContext()->setContextProperty("organizationViewModel", organizationViewModel);
    engine.rootContext()->setContextProperty("userViewModel", userViewModel);
    engine.rootContext()->setContextProperty("updateOrganizationViewModel", updateOrganizationViewModel);
    engine.rootContext()->setContextProperty("userUpdateViewModel", userUpdateViewModel);
    engine.rootContext()->setContextProperty("createAnimalViewModel", createAnimalViewModel);
    engine.rootContext()->setContextProperty("createReviewViewModel", createReviewViewModel);
    engine.rootContext()->setContextProperty("organizationCardViewModel", organizationCardViewModel);
    engine.rootContext()->setContextProperty("animalDetailViewModel", animalDetailViewModel);
    engine.rootContext()->setContextProperty("searchOrganizationViewModel", searchOrganizationViewModel);
    engine.rootContext()->setContextProperty("updateAnimalViewModel", updateAnimalViewModel);
    engine.rootContext()->setContextProperty("animalListViewModel", animalListViewModel);
    engine.rootContext()->setContextProperty("createPostViewModel", createPostViewModel);
    engine.rootContext()->setContextProperty("reviewListViewModel", reviewListViewModel);
    engine.rootContext()->setContextProperty("updateReviewViewModel", updateReviewViewModel);

    engine.rootContext()->setContextProperty("postListViewModel", postListViewModel);
    engine.rootContext()->setContextProperty("postCardViewModel", postCardViewModel);
    engine.rootContext()->setContextProperty("adoptRequestListViewModel", adoptRequestListViewModel);
    engine.rootContext()->setContextProperty("updatePostViewModel", updatePostViewModel);
    engine.rootContext()->setContextProperty("photoUploadViewModel", photoUploadViewModel);
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