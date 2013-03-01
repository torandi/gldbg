#!/bin/sh

LIBGLDBG_PATH=/home/torandi/dev/gldbg/build

LD_PRELOAD="$LIBGLDBG_PATH/libgldbg.so.0" $@
