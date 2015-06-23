set(PROJECT "rfds-power")
project(${PROJECT})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)

file(GLOB SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/service/api)

add_executable(${PROJECT} ${SOURCE_FILES} ) 

target_link_libraries(${PROJECT} Qt5::Core pthread Qt5::Network ${CMAKE_BINARY_DIR}/service/api/librfds-api.a ${CMAKE_BINARY_DIR}/common/librfds-common.a)

