cmake_minimum_required(VERSION 3.1.0)
set(PROJECT "refude")
project(${PROJECT})

add_subdirectory(test)

set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-rdynamic")  

include(GNUInstallDirs)

file(GLOB SOURCE_FILES impl/*.cpp)

file(GLOB PUBLIC_HEADERS *.h)

add_library(${PROJECT} SHARED ${SOURCE_FILES} )

install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_LIBDIR})

