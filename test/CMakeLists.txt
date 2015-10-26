project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(test_socket_speed test_socket_speed.cpp) 
target_link_libraries(test_socket_speed pthread ${CMAKE_BINARY_DIR}/lib/librestfulipc.so)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data.json ${CMAKE_CURRENT_BINARY_DIR}/data.json)

add_executable(test_json_parser test_json_parser.cpp data.json)
target_link_libraries(test_json_parser pthread ${CMAKE_BINARY_DIR}/lib/librestfulipc.so)
