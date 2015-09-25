add_definitions(
	--std=c++14
)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	add_definitions( -fPIC)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(
		-Werror
		-g
	)
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")

file(GLOB LIBALGAUDIO_SOURCES
	./*.cpp
	./UI/*.cpp
	./ModuleUI/*.cpp
	./SDLFix/*.cpp
	./BuiltinModules/*.cpp
)
file(GLOB LIBALGAUDIO_INCLUDES ./include/*.hpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Version.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Version.hpp @ONLY)
	
include_directories(
	"./include"
	"${CMAKE_SOURCE_DIR}/libs"
	${SDL2_INCLUDE_DIR}
	${SDL2_TTF_INCLUDE_DIR}
)

# Suppress warnings from included headers
include_directories( SYSTEM
	${LIBLO_INCLUDE_DIR}
)

link_directories(
	${CMAKE_BINARY_DIR}
	${SDL2_LIBRARY_DIR}
	${SDL2_TTF_LIBRARY_DIR}
)

add_library(
	libalgaudio SHARED
	${LIBALGAUDIO_SOURCES}
  ${LIBALGAUDIO_INCLUDES}
)

add_dependencies(libalgaudio liblo nativefiledialog)

# Disables the addition of one more "lib" prefix on libalgaudio
SET_TARGET_PROPERTIES(libalgaudio PROPERTIES PREFIX "")

target_link_libraries(
	libalgaudio
	${SDL2_LIBRARY}
	${SDL2_TTF_LIBRARY}
	${LIBLO_LIBRARY}
	${NFD_LIBRARY}
	freetype
	${EXTRA_SYS_LIBS}
)
# The following command will clear transitive dependencies of libalgaudio.
# This way plugins (which depend on libalgaudio) will not be linked against
# libSDL
SET_TARGET_PROPERTIES(libalgaudio PROPERTIES INTERFACE_LINK_LIBRARIES "")

add_custom_command(
	TARGET libalgaudio POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libalgaudio${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/"
)
