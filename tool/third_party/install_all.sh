#!/bin/bash

if [ $# -eq 0 ]
then
    bash install_antlr.sh
    bash install_spotltl.sh
    bash install_z3.sh
    bash install_gedlib.sh
else
    installPrefix="$1"
    bash install_antlr.sh "$installPrefix"
    bash install_spotltl.sh "$installPrefix"
    bash install_z3.sh "$installPrefix"
    bash install_gedlib..sh "$installPrefix"
    bash install_hifsuite.sh "$installPrefix"
    bash install_os-cad.sh "$installPrefix"
fi


