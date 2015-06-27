set(PROJECT "rfds-common")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT} ${SOURCE_FILES} ) 


