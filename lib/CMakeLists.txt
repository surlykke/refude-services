cmake_minimum_required(VERSION 3.1.0)
set(PROJECT "refude")
project(${PROJECT})

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}") 
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-rdynamic")  

include(GNUInstallDirs)

file(GLOB SOURCE_FILES impl/abstractcachingresource.cpp impl/abstractresource.cpp impl/buffer.cpp impl/errorhandling.cpp impl/genericresource.cpp impl/httpmessage.cpp impl/httpprotocol.cpp impl/json.cpp impl/jsonreader.cpp impl/jsonwriter.cpp impl/jsonresource.cpp impl/collectionresource.cpp impl/localizingjsonwriter.cpp impl/methods.cpp impl/notifierresource.cpp impl/service.cpp impl/test.cpp impl/utils.cpp impl/webserver.cpp impl/shortmtqueue.h impl/map.h)
file(GLOB PUBLIC_HEADERS public_headers/*.h)

include_directories(public_headers private_headers)

add_library(${PROJECT} SHARED ${SOURCE_FILES} )

install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/refuce)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_LIBDIR})

