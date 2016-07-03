cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}") 

set(PROJECT "RefudeService")
project(${PROJECT})

include(GNUInstallDirs)
link_directories(/usr/local/lib64)
set(CMAKE_INCLUDE_CURRENT_DIR ON)



include_directories(common)
add_subdirectory(desktop-service)
add_subdirectory(icon-service)
add_subdirectory(running-applications)

#add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
#target_link_libraries(${PROJECT} pthread restfulipc tinyxml2 magic dl X11 png)
#
#install(FILES refudeservice.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
#install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
#
