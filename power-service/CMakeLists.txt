set(PROJECT "RefudePower")
project(${PROJECT})

set(CMAKE_AUTOMOC ON)
find_package(Qt5DBus REQUIRED)

set(SOURCE_FILES 
    main.cpp
    powerapplication.cpp
    batteryresource.cpp
    properties_if.cpp
    upower_if.cpp
    ../common/xdg.cpp
) 

add_executable(${PROJECT} ${SOURCE_FILES})

target_link_libraries(${PROJECT} pthread restfulipc dl magic Qt5::DBus)

#TODO install(FILES refudepower.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
#install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
