set(PROJECT "restfulipc")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp shortmtqueue.h)
add_library(${PROJECT} ${SOURCE_FILES} )

