set(PROJECT "RefudeDesktopService")
project(${PROJECT})

set(CMAKE_AUTOMOC ON)

#set(SOURCE_FILES desktopentryreader.cpp desktopentryresource.cpp desktopentryresourcesbuilder.cpp desktopservice.cpp desktopwatcher.cpp main.cpp mimeappslistreader.cpp mimeresourcebuilder.cpp mimetyperesource.cpp runapplication.cpp ../common/inireader.cpp ../common/xdg.cpp)

set(SOURCE_FILES 
    applicationcollector.cpp
    controller.cpp 
    desktopwatcher.cpp
    desktopentryreader.cpp
    main.cpp
    mimeappslistreader.cpp 
    mimetypecollector.cpp 
    runapplication.cpp 
    typedefs.h
    ../common/inireader.cpp 
    ../common/xdg.cpp)


add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})

target_link_libraries(${PROJECT} pthread restfulipc tinyxml2 dl magic)
#target_link_libraries(${PROJECT} pthread restfulipc magic dl X11 png)

#TODO install(FILES refudedesktopservice.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
