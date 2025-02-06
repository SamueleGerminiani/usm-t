#!/bin/bash

if [ $# -eq 0 ]
then
    bash install_antlr.sh
    bash install_spotltl.sh
    bash install_boost.sh
else
    installPrefix="$1"
    bash install_antlr.sh "$installPrefix"
    bash install_spotltl.sh "$installPrefix"
    bash install_boost.sh "$installPrefix"
    bash install_hifsuite.sh
    bash install_oss-cad.sh "$installPrefix"
fi


