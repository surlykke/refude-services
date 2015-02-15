# Reference: http://qt-project.org/doc/qt-5.0/qtdoc/cmake-manual.html

cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "rfds")

project(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)

set(CLIENT_SOURCE_FILES
    src/client/main.cpp
)

set(SERVICE_SOURCE_FILES
    src/service/main.cpp
	src/service/service_listener.cpp
)

add_executable(${PROJECT}-client 
    ${CLIENT_SOURCE_FILES}
)

add_executable(${PROJECT}-service 
    ${SERVICE_SOURCE_FILES}
)

target_link_libraries(${PROJECT}-service Qt5::Core Qt5::Network)

