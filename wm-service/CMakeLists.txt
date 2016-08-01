set(PROJECT "RefudeWmService")
project(${PROJECT})

set(SOURCE_FILES 
        main.cpp
        runningappsicons.cpp 
        windowinfo.cpp
		controller.cpp
        ../common/xdg.cpp
)

add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
target_link_libraries(${PROJECT} pthread restfulipc dl magic X11 png)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
