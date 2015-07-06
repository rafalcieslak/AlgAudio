
# For modules, disablee the "lib" prefix on libraries
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# All modules will want to use this dir
include_directories(../libalgaudio/include)

add_subdirectory(base)

add_custom_target(example.aa ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/example.xml ${CMAKE_BINARY_DIR}/modules/.
)
