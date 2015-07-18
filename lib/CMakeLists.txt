set(PROJECT "restfulipc")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT} ${SOURCE_FILES} )

target_link_libraries(${PROJECT} pthread)
