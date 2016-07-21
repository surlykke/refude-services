set(PROJECT "WindowManagerService")
project(${PROJECT})

set(SOURCE_FILES 
        main.cpp
#        runningapplicationsresource.cpp  
        runningappsicons.cpp 
#        displayresource.cpp 
        windowinfo.cpp
		controller.cpp
        ../common/xdg.cpp
)


add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})

target_link_libraries(${PROJECT} pthread restfulipc dl magic X11 png)
#target_link_libraries(${PROJECT} pthread restfulipc magic dl X11)

#TODO install(FILES refudedesktopservice.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
