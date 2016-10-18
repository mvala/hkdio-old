#!/bin/bash

SALSA_DIR="$(dirname $(dirname $(readlink -m $0)))"

[ "$1" = "clean" ] && { shift; rm -rf $SALSA_DIR/build; }
[ -d $SALSA_DIR/build ] ||  mkdir $SALSA_DIR/build

cd $SALSA_DIR/build || exit 1
[ -f Makefile ] || cmake -DCMAKE_INSTALL_PREFIX=$SALSA_DIR -DCMAKE_BUILD_TYPE=RelWithDebInfo ../
make -j$(nproc) $*

