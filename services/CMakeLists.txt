set(PROJECT "Services")
project(${PROJECT})

include_directories(common)
include_directories(../lib)
add_subdirectory(desktop-service)
add_subdirectory(icon-service)
add_subdirectory(wm-service)
add_subdirectory(power-service)
