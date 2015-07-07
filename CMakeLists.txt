cmake_minimum_required(VERSION 3.0)

project(
  AlgAudio
  VERSION 0.1
  LANGUAGES CXX C
)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

# Global config

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
#	set(EXTRA_SYS_LIBS wsock32 Ws2_32 comdlg32)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	message(STATUS "Using clang.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(libalgaudio)
add_subdirectory(algaudio)
add_subdirectory(modules)
