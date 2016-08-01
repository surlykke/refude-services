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
add_subdirectory(wm-service)
add_subdirectory(power-service)

install(FILES haproxy/haproxy.cfg DESTINATION /etc/refude)
install(FILES runRefude.sh 
	    DESTINATION ${CMAKE_INSTALL_BINDIR} 
		PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
		            OWNER_WRITE 
					OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
