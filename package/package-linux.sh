#!/bin/sh

# Run this package inside the build dir once everything has been compiled

DIR=pkg

mkdir -p $DIR
cp algaudio.bin libalgaudio.so $DIR
cp -r fonts $DIR
mkdir -p $DIR/modules
mkdir -p $DIR/sc
cp modules/*.xml $DIR/modules/.
cp modules/*.aa.so $DIR/modules/.
cp sc/*.sc $DIR/sc/.
cp -r ../examples $DIR 
