add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-g
)

file(GLOB MODULES_BASE_SOURCES ./*.cpp)

add_library(
	modules_base.aa SHARED
	${MODULES_BASE_SOURCES}
)

target_link_libraries(
	modules_base.aa
  libalgaudio
  ${EXTRA_SYS_LIBS}
)

add_custom_command(
	TARGET modules_base.aa POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/modules_base.aa${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/modules"
)
