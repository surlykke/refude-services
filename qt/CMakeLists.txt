set(PROJECT "qt")
project(${PROJECT})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)

file(GLOB SOURCE_FILES *.cpp) 

add_executable(${PROJECT} ${SOURCE_FILES})

target_link_libraries(${PROJECT} Qt5::Core Qt5::Network)

