#!/bin/sh

LIBGLDBG_PATH=/usr/local/lib

LD_PRELOAD="$LIBGLDBG_PATH/libgldbg.so.0" $@
