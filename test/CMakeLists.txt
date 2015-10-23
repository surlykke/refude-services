project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(test_socket_speed test_socket_speed.cpp) 
target_link_libraries(test_socket_speed pthread ${CMAKE_BINARY_DIR}/lib/librestfulipc.so)

