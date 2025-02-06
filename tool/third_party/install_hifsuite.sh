#!/bin/bash

if [ -d "hifsuite_linux_x86_stand_alone" ]; then
    echo "Repository already exists."
    exit 0
fi

git clone https://github.com/SamueleGerminiani/hifsuite_linux_x86_stand_alone.git




