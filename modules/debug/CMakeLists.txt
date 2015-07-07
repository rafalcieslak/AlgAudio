add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-g
)

file(GLOB MODULES_DEBUG_SOURCES ./*.cpp)

add_library(
	debug.aa SHARED
	${MODULES_DEBUG_SOURCES}
)

target_link_libraries(
	debug.aa
  libalgaudio
  ${EXTRA_SYS_LIBS}
)

add_custom_command(
	TARGET debug.aa POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/debug.aa${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/modules"
)

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/modules/debug.xml"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/debug.xml" "${CMAKE_BINARY_DIR}/modules"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/debug.xml"
)

add_custom_target(copy_debug_xml DEPENDS "${CMAKE_BINARY_DIR}/modules/debug.xml")
add_custom_target(modules_debug ALL)
add_dependencies(modules_debug copy_debug_xml)
add_dependencies(modules_debug debug.aa)
