set(PROJECT "rfds-common")
project(${PROJECT})

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}") 
file(GLOB SOURCE_FILES *.cpp)
message("SOURCE_FILES ${SOURCE_FILES}")
add_library(${PROJECT} ${SOURCE_FILES} ) 


