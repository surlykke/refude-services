set(PROJECT "RefudeIconService")
project(${PROJECT})

set(SOURCE_FILES
		iconcollector.cpp
		iconresourcebuilder.cpp
		iconresource.cpp
		main.cpp
		themereader.cpp
		../common/xdg.cpp
		../common/inireader.cpp
)

add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
target_link_libraries(${PROJECT} pthread restfulipc dl magic)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
