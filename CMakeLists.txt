cmake_minimum_required(VERSION 3.1.0)
set(PROJECT "DesktopService")
project(${PROJECT})

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}") 

include(GNUInstallDirs)
link_directories(/usr/local/lib64)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB SOURCE_FILES *.cpp)

file(GLOB JSON_TEMPLATES *.json)
message(STATUS "JSON_TEMPLATES: ${JSON_TEMPLATES}")
foreach (JSON_TEMPLATE ${JSON_TEMPLATES}) 
    get_filename_component(BASENAME ${JSON_TEMPLATE} NAME_WE)
    set(HFILE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.h")
    set(CMD "${CMAKE_SOURCE_DIR}/toString.sh")
    add_custom_command(OUTPUT ${HFILE} COMMAND ${CMD} ARGS ${JSON_TEMPLATE} ${HFILE} DEPENDS ${JSON_TEMPLATE}) 
    list(APPEND HFILES ${HFILE})
endforeach (JSON_TEMPLATE) 

add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
target_link_libraries(${PROJECT} pthread restfulipc tinyxml2 magic dl)

install(FILES refude-services.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
