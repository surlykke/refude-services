set(PROJECT "RefudeDesktopService")
set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")
project(${PROJECT})

file(GLOB SOURCE_FILES *.cpp typedefs.h)
include_directories(../lib)
add_executable(${PROJECT} ${SOURCE_FILES} ${HFILES})
target_link_libraries(${PROJECT} pthread refude tinyxml2 dl magic)
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_BINDIR})
