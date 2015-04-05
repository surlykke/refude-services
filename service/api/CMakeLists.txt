set(PROJECT "rfds-api")
project(${PROJECT})

find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)

file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT} ${SOURCE_FILES})

message("CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

target_link_libraries(${PROJECT} pthread ${CMAKE_BINARY_DIR}/common/librfds-common.a)
