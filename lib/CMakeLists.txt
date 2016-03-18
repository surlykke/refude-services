set(PROJECT "restfulipc")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp shortmtqueue.h map.h list.h, stacktrace.h)
add_library(${PROJECT} SHARED ${SOURCE_FILES} )

