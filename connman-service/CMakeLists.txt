set(PROJECT "RefudeConnmanService")
project(${PROJECT})

set(CMAKE_AUTOMOC ON)
find_package(Qt5DBus REQUIRED)

file(GLOB SOURCE_FILES *.cpp)
include_directories(../lib)
add_executable(${PROJECT} ${SOURCE_FILES})
target_link_libraries(${PROJECT} pthread refude dl magic Qt5::DBus)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
