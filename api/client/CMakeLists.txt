set(PROJECT "rfds-client")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

add_library(${PROJECT} ${SOURCE_FILES} ) 

target_link_libraries(${PROJECT} ${CMAKE_BINARY_DIR}/api/common/librfds-common.a)

