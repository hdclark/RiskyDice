#!/usr/bin/env bash

rm -rf build &>/dev/null

set -e

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ../
#make && sudo make install
make

