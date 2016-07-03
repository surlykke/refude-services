set(PROJECT "RefudeDesktopService")
project(${PROJECT})

set(CMAKE_AUTOMOC ON)
set(SOURCE_FILES desktopentryreader.cpp desktopentryresource.cpp desktopentryresourcesbuilder.cpp desktopservice.cpp desktopwatcher.cpp main.cpp mimeappslistreader.cpp mimeresourcebuilder.cpp mimetyperesource.cpp runapplication.cpp ../common/inireader.cpp ../common/xdg.cpp)

set(JSON_TEMPLATES commandsTemplate.json desktopActionTemplate.json handlerTemplate.json subtypeTemplate.json commandTemplate.json desktopTemplate.json rootTemplate.json)

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

target_link_libraries(${PROJECT} pthread restfulipc tinyxml2 dl magic)
#target_link_libraries(${PROJECT} pthread restfulipc magic dl X11 png)

#TODO install(FILES refudedesktopservice.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
