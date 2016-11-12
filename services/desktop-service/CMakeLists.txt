set(PROJECT "RefudeDesktopService")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp typedefs.h ../common/inireader.cpp ../common/xdg.cpp)

add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
target_link_libraries(${PROJECT} pthread refude tinyxml2 dl magic)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
