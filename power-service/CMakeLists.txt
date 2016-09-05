set(PROJECT "RefudePowerService")
project(${PROJECT})

set(CMAKE_AUTOMOC ON)
find_package(Qt5DBus REQUIRED)

set(SOURCE_FILES 
    main.cpp
    powerapplication.cpp
    properties_if.cpp
    upower_if.cpp
    ../common/xdg.cpp
) 

add_executable(${PROJECT} ${SOURCE_FILES})
target_link_libraries(${PROJECT} pthread restfulipc dl magic Qt5::DBus)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
