cmake_minimum_required(VERSION 3.1.0)
set(PROJECT "restfulipc")
project(${PROJECT})

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}") 
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-rdynamic")  

include(GNUInstallDirs)

file(GLOB SOURCE_FILES impl/*.cpp impl/shortmtqueue.h impl/map.h)
file(GLOB PUBLIC_HEADERS public_headers/*.h)

include_directories(public_headers private_headers)

add_library(${PROJECT} SHARED ${SOURCE_FILES} )

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/ripc)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_LIBDIR})
