set(PROJECT "rfds-api")
project(${PROJECT})

find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)

file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT} ${SOURCE_FILES})

target_link_libraries(${PROJECT} pthread)
