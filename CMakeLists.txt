cmake_minimum_required(VERSION 3.0)

include(cmake/CopyMacros.cmake)

project(
  AlgAudio
  VERSION 0.1
  LANGUAGES CXX C
)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Global config

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
	set(EXTRA_SYS_LIBS dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid  wsock32 Ws2_32 comdlg32)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	message(STATUS "Using clang.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# The libs subdir has to be processed first
add_subdirectory(libs)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(libalgaudio)
add_subdirectory(algaudio)
add_subdirectory(modules)

add_custom_target(
  sc_scripts ALL
)
copy_directory_if_changed(${PROJECT_SOURCE_DIR}/sc ${PROJECT_BINARY_DIR}/sc sc_scripts)

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
