project(${PROJECT})

include_directories(..)

file(GLOB SOURCE_FILES *.cpp)


add_executable(test_socket_speed test_socket_speed.cpp) 
target_link_libraries(test_socket_speed refude pthread magic dl)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data.json ${CMAKE_CURRENT_BINARY_DIR}/data.json)

add_executable(test_json test_json.cpp)
target_link_libraries(test_json refude pthread magic dl)

add_executable(test_map test_map.cpp)
target_link_libraries(test_map refude pthread magic dl)

add_executable(test_json_2 test_json_2.cpp)
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
target_link_libraries(test_json_2 refude pthread magic dl)

add_executable(test_json_parser test_json_parser.cpp data.json)
target_link_libraries(test_json_parser refude pthread magic dl)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/string.json ${CMAKE_CURRENT_BINARY_DIR}/string.json)
add_executable(test_string_escaping test_string_escaping.cpp data.json)
target_link_libraries(test_string_escaping refude pthread magic dl)

