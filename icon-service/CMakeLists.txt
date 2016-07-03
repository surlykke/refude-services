set(PROJECT "RefudeIconService")
project(${PROJECT})

set(SOURCE_FILES iconcollector.cpp iconresourcebuilder.cpp iconresource.cpp main.cpp themereader.cpp ../common/xdg.cpp ../common/inireader.cpp) 
set(JSON_TEMPLATES themesTemplate.json  themeTemplate.json)

#message(STATUS "JSON_TEMPLATES: ${JSON_TEMPLATES}")
foreach (JSON_TEMPLATE ${JSON_TEMPLATES}) 
    get_filename_component(BASENAME ${JSON_TEMPLATE} NAME_WE)
    set(HFILE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.h")
    set(JSON_TEMPLATE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${JSON_TEMPLATE}")
    set(CMD "${CMAKE_SOURCE_DIR}/toString.sh")
    add_custom_command(OUTPUT ${HFILE} COMMAND ${CMD} ARGS ${JSON_TEMPLATE_PATH} ${HFILE} DEPENDS ${JSON_TEMPLATE}) 
    list(APPEND HFILES ${HFILE})
endforeach (JSON_TEMPLATE) 

add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})

target_link_libraries(${PROJECT} pthread restfulipc dl magic)
#target_link_libraries(${PROJECT} pthread restfulipc magic dl X11 png)

#TODO install(FILES refudedesktopservice.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
