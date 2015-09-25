# AlgAudio

AlgAudio is an audio processing framework, designed to be easy to learn and use at live performances.

![screenshot](http://i.imgur.com/JQU8xWD.png)

Copyright (C) 2015 CeTA - Audiovisual Technology Center

Algaudio is released under the terms of the GNU Lesser General Public License version 3, see COPYING and COPYING.LESSER for details.

Module specification draft is available in `./doc/modules.md` ([see it here](https://github.com/rafalcieslak/AlgAudio/blob/master/doc/modules.md)).

The API documentation can be built with `make doc`, [or browsed online](http://cielak.org/algaudio/doc).

# Building and running

## On Linux

To build, you will need CMake 3.0 or newer, scons, git, and development headers for SDL2 2.0.3 and SDL2-ttf, and freetype. If you wish to build documentation, you will also need doxygen. Run:

	mkdir build
	cd build
	cmake ..
	make

To launch, you will also need sclang (SuperCollider). Execute [from `build` directory]:

	./algaudio.bin

## On Windows

The supported way of building AlgAudio on Windows is to use MinGW toolchain in MSYS2 environment.

From MSYS2 shell, use `pacman` to install following packages (choose an architecture you are compiling for):

	mingw/gcc
	mingw/make
	mingw/SDL2
	mingw/SDL2_ttf
	mingw/cmake
	mingw/freetype
	make
	automake
	patch
	git
	scons
	pkg-config

Note that even if you have CMake or gcc installed on your host system, you still need to install them in your MSYS environment.

In project directory, run:

	mkdir build
	cd build
	cmake -G "MSYS Makefiles" ..
	make

When ready, run `./algaudio.exe` to launch the application.

If you would like to create a standalone redistributable version, pass `-DCMAKE_BUILD_TYPE=Release` to `cmake`. After complete build, use `make pkg` to collect all files (including dll dependencies needed to redistribute AlgAudio) into `package` directory.

## On other systems

While it was not currently tested, compiling the application on other systems should be also possible, because no platform-specific CMake modules are used. Please refer to CMake documentation for your platform to learn how to generate build files using CMake.
