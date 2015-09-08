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

# Global config


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	message(STATUS "Using clang.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf-2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2")
endif(${CMAKE_BUILD_TYPE} MATCHES "Debug")

find_package(PkgConfig REQUIRED)

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

# The libs subdir has to be processed first
add_subdirectory(libs)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(libalgaudio)
add_subdirectory(algaudio)
add_subdirectory(modules)
add_subdirectory(sc)
add_subdirectory(doc)

add_custom_target(
  package-windows
  COMMAND sh ${PROJECT_SOURCE_DIR}/package/package-windows.sh
)
add_dependencies(
  package-windows
  libalgaudio
  liblo
  modules_base
  modules_debug
  algaudio
)
