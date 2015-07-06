add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-g
)

file(GLOB LIBALGAUDIO_SOURCES ./*.cpp)
file(GLOB LIBALGAUDIO_INCLUDES ./include/*.hpp)

include_directories("./include")
include_directories("${CMAKE_SOURCE_DIR}/libs")

add_library(
	libalgaudio SHARED
	${LIBALGAUDIO_SOURCES}
  ${LIBALGAUDIO_INCLUDES}
)
# Disables the addition of one more "lib" prefix on libalgaudio
SET_TARGET_PROPERTIES(libalgaudio PROPERTIES PREFIX "")

target_link_libraries(
	libalgaudio
	${EXTRA_SYS_LIBS}
)

add_custom_command(
	TARGET libalgaudio POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libalgaudio${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/"
)
