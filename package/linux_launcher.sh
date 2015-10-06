#!/bin/sh
SCRIPTPATH=$( cd $(dirname $0) ; pwd -P )
cd $SCRIPTPATH
export LD_LIBRARY_PATH=$SCRIPTPATH
$SCRIPTPATH/algaudio.bin
