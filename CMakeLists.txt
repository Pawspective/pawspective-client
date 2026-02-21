cmake_minimum_required(VERSION 3.21)

project(pawspective-client VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_PREFIX_PATH AND DEFINED ENV{QT_DIR})
	set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}" CACHE PATH "" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(src)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(pawspective-client
	src/main.cpp
	src/mainwindow.cpp
	${PROJECT_HEADERS}
)

target_include_directories(pawspective-client PRIVATE include)

target_link_libraries(pawspective-client PRIVATE Qt6::Widgets)
