set(PROJECT "rfds-server")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

message("Server add include dir ../common")
include_directories( ../common )

add_library(${PROJECT} ${SOURCE_FILES})

target_link_libraries(${PROJECT} pthread ${CMAKE_BINARY_DIR}/common/librfds-common.a)
