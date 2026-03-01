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

message($(CMAKE_PREFIX_PATH))
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
    src/viewmodels/base.cpp
	${PROJECT_HEADERS}
)

qt_add_qml_module(pawspective-client
    URI pawspective
    VERSION 1.0
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
        resources/reg_dog.png
)

target_include_directories(pawspective-client PRIVATE include)

target_link_libraries(pawspective-client PRIVATE
    Qt6::Core
    Qt6::Gui
	Qt6::Network
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)
