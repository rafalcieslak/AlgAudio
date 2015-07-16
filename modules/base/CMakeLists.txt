add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-g
)

file(GLOB MODULES_BASE_SOURCES ./*.cpp)

# Suppress warnings from included headers
include_directories( SYSTEM
	${LIBLO_INCLUDE_DIR}
)

add_library(
	base.aa SHARED
	${MODULES_BASE_SOURCES}
)

target_link_libraries(
	base.aa
	libalgaudio
  ${EXTRA_SYS_LIBS}
)

add_custom_command(
	TARGET base.aa POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/base.aa${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/modules"
)

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/modules/base.xml"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/base.xml" "${CMAKE_BINARY_DIR}/modules"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/base.xml"
)

add_custom_target(copy_base_xml DEPENDS "${CMAKE_BINARY_DIR}/modules/base.xml")
add_custom_target(modules_base ALL)
add_dependencies(modules_base copy_base_xml)
add_dependencies(modules_base base.aa)
