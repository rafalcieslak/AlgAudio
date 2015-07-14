find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_definitions(
	--std=c++14
	-Wall
	-Wextra
	-g
)

file(GLOB LIBALGAUDIO_SOURCES ./*.cpp ./UI/*.cpp)
file(GLOB LIBALGAUDIO_INCLUDES ./include/*.hpp)

include_directories(
	"./include"
	"${CMAKE_SOURCE_DIR}/libs"
	${SDL2_INCLUDE_DIR}
	${SDL2_TTF_INCLUDE_DIR}
	${LIBLO_INCLUDE_DIR}
)

link_directories(
	${SDL2_LIBRARY_DIR}
	${SDL2_TTF_LIBRARY_DIR}
)

add_library(
	libalgaudio SHARED
	${LIBALGAUDIO_SOURCES}
  ${LIBALGAUDIO_INCLUDES}
)

add_dependencies(libalgaudio liblo)


# Disables the addition of one more "lib" prefix on libalgaudio
SET_TARGET_PROPERTIES(libalgaudio PROPERTIES PREFIX "")

target_link_libraries(
	libalgaudio
	${SDL2_LIBRARY}
	${SDL2_TTF_LIBRARY}
	${LIBLO_LIBRARY}
	${EXTRA_SYS_LIBS}
)
# The following command will clear transitive dependencies of libalgaudio.
# This way plugins (which depend on libalgaudio) will not be linked agains
# libSDL
SET_TARGET_PROPERTIES(libalgaudio PROPERTIES INTERFACE_LINK_LIBRARIES "")

add_custom_command(
	TARGET libalgaudio POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libalgaudio${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/"
)
