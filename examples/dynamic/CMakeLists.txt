project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/lib/client ${CMAKE_SOURCE_DIR}/lib/server ${CMAKE_SOURCE_DIR}/lib/common)

add_executable(dynamic_resource dynamic_resource.cpp) 
target_link_libraries(dynamic_resource pthread ${CMAKE_BINARY_DIR}/lib/librestfulipc.a)

add_executable(dynamic_client dynamic_client.cpp) 
target_link_libraries(dynamic_client pthread  ${CMAKE_BINARY_DIR}/lib/librestfulipc.a)

