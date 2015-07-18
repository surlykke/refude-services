set(PROJECT "restfulipc")
project(${PROJECT})

file(GLOB SOURCE_FILES client/*.cpp common/*.cpp server/*.cpp)
include_directories(common)

add_library(${PROJECT} ${SOURCE_FILES} )

target_link_libraries(${PROJECT} pthread)
