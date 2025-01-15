#!/bin/bash

NThreads=1

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    NThreads="`grep -c ^processor /proc/cpuinfo`"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    NThreads="`sysctl -n hw.ncpu`"
fi

if [ $# -eq 0 ]
then
    installPrefix="$(pwd)/z3"
    mkdir z3
else
    installPrefix="$1"
fi


wget --no-check-certificate https://github.com/Z3Prover/z3/archive/refs/tags/z3-4.8.10.tar.gz
tar -xvf z3-4.8.10.tar.gz && rm z3-4.8.10.tar.gz
cd z3-z3-4.8.10
./configure --staticlib --prefix "$installPrefix"
cd build
make -j"$NThreads"
make install
cd ../../
rm -rf z3-z3-4.8.10
