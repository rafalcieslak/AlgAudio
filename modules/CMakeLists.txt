
# For modules, disablee the "lib" prefix on libraries
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# All modules will want to use this dir
include_directories(../libalgaudio/include)

add_subdirectory(base)
