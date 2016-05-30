#!/bin/bash

HK_DIR="$(dirname $(dirname $(readlink -m $0)))"

[ "$1" = "clean" ] && rm -rf $HK_DIR/build
[ -d $HK_DIR/build ] ||  mkdir $HK_DIR/build

[ -n "$ALICE_ROOT" ] && HK_ALICE="-DALIROOT=$ALICE_ROOT"

cd $HK_DIR/build || exit 1
cmake -DCMAKE_INSTALL_PREFIX=$HK_DIR -DCMAKE_BUILD_TYPE=RelWithDebInfo $HK_ALICE ../
make -j$(nproc) install

