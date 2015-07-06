add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-g
)

file(GLOB ALGAUDIO_SOURCES ./*.cpp)

include_directories(../libalgaudio/include)

add_executable(
	algaudio
	${ALGAUDIO_SOURCES}
)

target_link_libraries(
	algaudio
  libalgaudio
	${EXTRA_SYS_LIBS}
)

add_custom_command(
	TARGET algaudio POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/algaudio${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_BINARY_DIR}/"
)
