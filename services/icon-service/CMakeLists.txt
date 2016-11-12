set(PROJECT "RefudeIconService")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp ../common/*.cpp)

add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
target_link_libraries(${PROJECT} pthread refude dl magic)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
