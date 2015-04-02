set(PROJECT "rfds-internal")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT} ${SOURCE_FILES} ) 


