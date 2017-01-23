cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

set(PROJECT "RefudeService")
project(${PROJECT})

if(NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE Debug )
endif (NOT CMAKE_BUILD_TYPE)

include(GNUInstallDirs)
link_directories(/usr/local/lib64)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(lib)
add_subdirectory(desktop-service)
add_subdirectory(icon-service)
add_subdirectory(wm-service)
add_subdirectory(power-service)
add_subdirectory(connman-service)

install(FILES haproxy/haproxy.cfg DESTINATION /etc/refude)
install(FILES bin/runRefude.sh bin/refudeHTTP
	    DESTINATION ${CMAKE_INSTALL_BINDIR} 
		PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
		            OWNER_WRITE 
					OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
