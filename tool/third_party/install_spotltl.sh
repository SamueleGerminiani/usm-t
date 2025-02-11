#!/bin/bash

NThreads=1

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    NThreads="`grep -c ^processor /proc/cpuinfo`"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    NThreads="`sysctl -n hw.ncpu`"
fi

if [ $# -eq 0 ]
then
    installPrefix="$(pwd)/spot"
    mkdir spot
else
    installPrefix="$1"
fi


wget --no-check-certificate http://www.lrde.epita.fr/dload/spot/spot-2.12.2.tar.gz
tar -xvf spot-2.12.2.tar.gz && rm spot-2.12.2.tar.gz
cd spot-2.12.2
./configure --disable-python --prefix "$installPrefix"
make -j"$NThreads"
make install
cd ../
rm -rf spot-2.12.2
