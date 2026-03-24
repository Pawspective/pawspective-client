cmake_minimum_required(VERSION 3.21)

project(pawspective-client VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_PREFIX_PATH AND DEFINED ENV{QT_DIR})
 set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}" CACHE PATH "" FORCE)
endif()


find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
	Network
    Widgets
    Qml
    Quick
    QuickLayouts
    QuickControls2
)


file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
 "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(pawspective-client
	src/main.cpp
	src/mainwindow.cpp
	src/utils/validator.cpp
	src/models/user_dto.cpp
	src/models/user_update_dto.cpp
	src/models/user_register_dto.cpp
	src/services/network_client.cpp
    src/services/auth_service.cpp
    src/services/user_service.cpp
    src/services/organization_service.cpp
    src/services/city_service.cpp
    src/services/animal_service.cpp
    src/services/errors.cpp
    src/viewmodels/base.cpp
    src/viewmodels/user_viewmodel.cpp
    src/viewmodels/user_update_viewmodel.cpp
    src/viewmodels/login_view_model.cpp
    src/viewmodels/register_view_model.cpp
    src/viewmodels/register_organization_view_model.cpp
    src/viewmodels/update_organization_viewmodel.cpp
    src/models/organization_dto.cpp
	src/models/organization_update_dto.cpp
	src/models/organization_register_dto.cpp
    src/models/city_dto.cpp
    src/models/animal_enums.cpp
    src/models/breed_dto.cpp
    src/models/animal_dto.cpp
    src/models/animal_register_dto.cpp
    src/models/animal_update_dto.cpp
    src/models/animal_filter_dto.cpp
    src/utils/json.cpp
    src/viewmodels/organization_view_model.cpp
	${PROJECT_HEADERS}
)

qt_add_qml_module(pawspective-client
    URI pawspective
    VERSION 1.0
    NO_CACHEGEN
    QML_FILES
        qml/Main.qml
        qml/UserView.qml
		resources/blue_cat.png
		resources/lilac.png
		qml/LoginView.qml
        qml/RegisterView.qml
		qml/LoaderSpinner.qml
        qml/CustomButton.qml
        resources/loading_paw.png
        resources/tricky_cat.png
		qml/UserUpdateView.qml
        qml/RegisterOrganizationView.qml
        qml/UpdateOrganizationView.qml
        qml/OrganizationView.qml
        resources/sad_cat.png
        resources/reg_dog.png
        resources/heart_dog.png
        qml/AnimalCreateView.qml
)

target_include_directories(pawspective-client PRIVATE include)

target_link_libraries(pawspective-client PRIVATE
    Qt6::Core
    Qt6::Gui
	Qt6::Network
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickLayouts
    Qt6::QuickControls2
)

# Tests
find_package(Qt6 REQUIRED COMPONENTS Test)
enable_testing()

add_executable(organization_service_test
    tests/organization_service_test.cpp
    include/services/organization_service.hpp
    src/utils/validator.cpp
    src/models/organization_dto.cpp
    src/models/organization_register_dto.cpp
    src/models/organization_update_dto.cpp
    src/models/city_dto.cpp
    src/services/organization_service.cpp
    src/services/errors.cpp
)

target_include_directories(organization_service_test PRIVATE include)

target_link_libraries(organization_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
)

add_test(NAME organization_service_test COMMAND organization_service_test)

add_executable(animal_service_test
    tests/animal_service_test.cpp
    include/services/animal_service.hpp
    src/models/animal_dto.cpp
    src/models/animal_enums.cpp
    src/models/animal_filter_dto.cpp
    src/models/animal_register_dto.cpp
    src/models/animal_update_dto.cpp
    src/models/breed_dto.cpp
    src/services/animal_service.cpp
    src/services/errors.cpp
    src/utils/json.cpp
)

target_include_directories(animal_service_test PRIVATE include)

target_link_libraries(animal_service_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Test
)

add_test(NAME animal_service_test COMMAND animal_service_test)

