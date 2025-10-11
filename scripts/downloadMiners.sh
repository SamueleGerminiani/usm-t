#!/bin/bash

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

bash $USMT_ROOT/miners/tools/goldminer/docker/download_image.sh 
bash $USMT_ROOT/miners/tools/texada/docker/download_image.sh 
bash $USMT_ROOT/miners/tools/harm/docker/download_image.sh 
bash $USMT_ROOT/miners/tools/samples2ltl/docker/download_image.sh 
bash $USMT_ROOT/miners/tools/gemini-flash/docker/download_image.sh 
