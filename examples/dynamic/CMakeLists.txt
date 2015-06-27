project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/api/client ${CMAKE_SOURCE_DIR}/api/server ${CMAKE_SOURCE_DIR}/api/common)

add_executable(dynamic_resource dynamic_resource.cpp) 
target_link_libraries(dynamic_resource pthread ${CMAKE_BINARY_DIR}/api/server/librfds-server.a ${CMAKE_BINARY_DIR}/api/common/librfds-common.a)

add_executable(dynamic_client dynamic_client.cpp) 
target_link_libraries(dynamic_client pthread ${CMAKE_BINARY_DIR}/api/client/librfds-client.a ${CMAKE_BINARY_DIR}/api/common/librfds-common.a)

