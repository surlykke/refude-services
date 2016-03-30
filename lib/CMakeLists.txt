set(PROJECT "restfulipc")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp shortmtqueue.h map.h list.h, stacktrace.h)
add_library(${PROJECT} SHARED ${SOURCE_FILES} )
include(GNUInstallDirs)
set(PUBLIC_HEADERS json.h jsonreader.h jsonwriter.h 
                   jsonresource.h abstractresource.h notifierresource.h
                   service.h shortmtqueue.h webserver.h
                   httpprotocol.h httpmessage.h 
                   errorhandling.h map.h buffer.h
                   utils.h) 
install(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/ripc)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_LIBDIR})