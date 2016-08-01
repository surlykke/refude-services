set(PROJECT "RefudeDesktopService")
project(${PROJECT})

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
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
