set(PROJECT "restfulipc")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

find_package(Boost REQUIRED COMPONENTS system thread)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
add_library(${PROJECT} ${SOURCE_FILES} )
#target_link_libraries(${PROJECT} pthread ${Boost_LIBRARIES})

