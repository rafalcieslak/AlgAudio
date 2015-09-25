#!/bin/sh

## This file demonstrates how you can build custom modules to link them to
## the SDK.

## Set SDK_DIR to the path where you put the SDK
SDK_DIR=./SDK/
NAME=example
## Uncomment depending on your platform.
# EXTENSION=.dll
# EXTENSION=.so

g++ -lalgaudio \
    -shared \
    -std=c++14 \
    -L$SDK_DIR \
    -I$SDK_DIR/include/ \
    $NAME.cpp \
    -o $NAME.aa$EXTENSION

## Afterwards, place both xml and dll/so files into algaudio's `modules`
## directory, and it should be ready to go!
