
add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-gdwarf-2
)

file(GLOB ALGAUDIO_SOURCES ./*.cpp)

include_directories(
	../libalgaudio/include
	${SDL2_INCLUDE_DIR}
)
# Suppress warnings from included headers
include_directories( SYSTEM
	${LIBLO_INCLUDE_DIR}
)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()


add_executable(
	algaudio
	${ALGAUDIO_SOURCES}
)

link_directories(
	${SDL2_LIBRARY_DIR}
)

target_link_libraries(
	algaudio
  libalgaudio
	${SDL2_LIBRARY}
	${EXTRA_SYS_LIBS}
)

add_dependencies(algaudio libalgaudio)

if(UNIX)
	# The suffix is needed to distinguish between the binary and a directory in
	# the build dir that has the same name...
	set(CMAKE_EXECUTABLE_SUFFIX ".bin")
endif()

add_custom_command(
	TARGET algaudio POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/algaudio${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_BINARY_DIR}/"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/fonts" "${CMAKE_BINARY_DIR}/fonts"
)
