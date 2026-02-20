#!/bin/bash

in_path=$1
out_path=$2

# Check for required arguments
if [ -z "$in_path" ] || [ -z "$out_path" ]; then
    echo "Usage: run.sh <in_path> <out_path>"
    exit 1
fi

if [ ! -d "$in_path" ]; then
    echo "Error: Input path '$in_path' is not a directory"
    exit 1
fi

# Check if the output path is a directory
if [ ! -d "$out_path" ]; then
    echo "Error: Output '$out_path' is not a directory."
    exit 1
fi


# Copy all .csv files from the input directory to the new output directory
find "$in_path" -maxdepth 1 -type f -name "*.csv" -exec cp {} "$out_path" \;

