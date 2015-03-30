set(PROJECT "rfds-client")
project(${PROJECT})


file(GLOB SOURCE_FILES *.cpp)

add_executable(${PROJECT} ${SOURCE_FILES} ) 

target_link_libraries(${PROJECT} curl)

