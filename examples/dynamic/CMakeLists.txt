project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp)

find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(dynamic_resource dynamic_resource.cpp) 
target_link_libraries(dynamic_resource pthread ${CMAKE_BINARY_DIR}/lib/librestfulipc.a ${Boost_LIBRARIES})

add_executable(dynamic_client dynamic_client.cpp) 
target_link_libraries(dynamic_client pthread  ${CMAKE_BINARY_DIR}/lib/librestfulipc.a)

