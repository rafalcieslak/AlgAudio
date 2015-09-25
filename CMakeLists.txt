cmake_minimum_required(VERSION 3.0)


message(STATUS "Current build mode: ${CMAKE_BUILD_TYPE}")

IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

project(
  AlgAudio
  VERSION 0.1
  LANGUAGES CXX C
)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(PkgConfig REQUIRED)
find_package(Git REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Doxygen)

# Global config
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	message(STATUS "Using clang.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_definitions(
  -Wall
  -Wextra
)

add_definitions( -O3 )
# Enable optimisations
if((${CMAKE_BUILD_TYPE} MATCHES "Release") AND ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux"))
  add_definitions( -flto )
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf-2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2")
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(EXTRA_SYS_LIBS dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid  wsock32 Ws2_32 comdlg32 opengl32)
endif()
if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
  # Find GTK. NAtiveFileDialog requires it. Because NFD compiles as a static
  # library, libalgaudio has to be linked to GTK.
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
  # Also, add GL lib.
  set(EXTRA_SYS_LIBS GL ${GTK3_LIBRARIES})
endif()

# Version information
execute_process(
	COMMAND git describe --tags --dirty --always
	OUTPUT_VARIABLE ALGAUDIO_VERSION
)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  string(CONCAT ALGAUDIO_VERSION ${ALGAUDIO_VERSION} "-debug")
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")
string(REGEX REPLACE "\n" "" ALGAUDIO_VERSION ${ALGAUDIO_VERSION})
message(STATUS "Version: ${ALGAUDIO_VERSION}")

# The libs subdir has to be processed first as it sets some vars for other subdirs
add_subdirectory(libs)
add_subdirectory(libalgaudio)
add_subdirectory(algaudio)
add_subdirectory(modules)
add_subdirectory(sc)
add_subdirectory(doc)

# ----- PACKAGE TARGETS -----
if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
  set(PACKAGING_SCRIPT "${PROJECT_SOURCE_DIR}/package/package-windows.sh")
endif("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
  set(PACKAGING_SCRIPT "${PROJECT_SOURCE_DIR}/package/package-linux.sh")
endif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")

add_custom_target(
  pkg
  COMMAND sh ${PACKAGING_SCRIPT}
)
add_dependencies(
  pkg
  libalgaudio
  liblo
  modules_base
  modules_debug
  algaudio
)

# ----- SDK -----

add_custom_target(sdk_hpp
	${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/libalgaudio/include" "${CMAKE_BINARY_DIR}/SDK/include"
)
if(DOXYGEN_FOUND)
	add_custom_target(SDK
	        	${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/doc/html" "${CMAKE_BINARY_DIR}/SDK/doc"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libalgaudio${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/SDK/."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/package/buildexample.sh" "${CMAKE_BINARY_DIR}/SDK/."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/doc/modules.md" "${CMAKE_BINARY_DIR}/SDK/."
    COMMENT "Gathering SDK files"
		DEPENDS doc sdk_hpp libalgaudio
	)
else(DOXYGEN_FOUND)
	add_custom_target(SDK
		        ${CMAKE_COMMAND} -E echo "SDK generated without docs, because doxygen was not found."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libalgaudio${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_BINARY_DIR}/SDK/."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/package/buildexample.sh" "${CMAKE_BINARY_DIR}/SDK/."
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/doc/modules.md" "${CMAKE_BINARY_DIR}/SDK/."
    COMMENT "Gathering SDK files"
		DEPENDS sdk_hpp libalgaudio
	)
endif(DOXYGEN_FOUND)
