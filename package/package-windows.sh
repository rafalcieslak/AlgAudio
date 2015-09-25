#!/bin/sh

# Run this package inside the build dir once everything has been compiled

DIR=package

mkdir -p $DIR
cp algaudio.exe libalgaudio.dll liblo-7.dll $DIR
cp -r fonts $DIR
mkdir -p $DIR/modules
mkdir -p $DIR/sc
cp modules/*.xml $DIR/modules/.
cp modules/*.aa.dll $DIR/modules/.
cp sc/*.sc $DIR/sc/.
cp -r ../examples $DIR 

# Copy all necessary dlls
ldd $DIR/*.dll | grep mingw | cut -d ' ' -f 3 | xargs -i cp {} $DIR/.
# Repeat few times for transitive dependencies
ldd $DIR/*.dll | grep mingw | cut -d ' ' -f 3 | xargs -i cp {} $DIR/.
ldd $DIR/*.dll | grep mingw | cut -d ' ' -f 3 | xargs -i cp {} $DIR/.
ldd $DIR/*.dll | grep mingw | cut -d ' ' -f 3 | xargs -i cp {} $DIR/.
