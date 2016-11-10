set(PROJECT "StupidWebserver")
project(${PROJECT})
include_directories(${CMAKE_SOURCE_DIR}/lib)
file(GLOB SOURCE_FILES *.cpp)

add_executable(${PROJECT} ${SOURCE_FILES})
target_link_libraries(${PROJECT} pthread ${CMAKE_BINARY_DIR}/lib/librestfulipc.so magic)
