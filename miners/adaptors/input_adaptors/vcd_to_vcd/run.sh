#!/bin/bash

in_path=$1
out_path=$2

# Check for required arguments
if [ -z "$in_path" ] || [ -z "$out_path" ]; then
    echo "Usage: run.sh <in_path> <out_path>"
    exit 1
fi

# Check if the output path is a directory
if [ ! -d "$out_path" ]; then
    echo "Error: Output 'out_path' is not a directory."
    exit 1
fi

# Check if input is a file
if [ -f "$in_path" ]; then
    # Check if the input file has a .vcd extension
    if [[ "$in_path" == *.vcd ]]; then
        cp "$in_path" "$out_path"
    else
        echo "Error: Input file '$in_path' does not have a .vcd extension."
        exit 1
    fi
# Check if input is a directory
elif [ -d "$in_path" ]; then
    # Create a new directory in the output path with the same name as the input directory
    base_name=$(basename "$in_path")
    new_out_dir="$out_path/$base_name"
    mkdir -p "$new_out_dir"

    # Copy all .vcd files from the input directory to the new output directory
    find "$in_path" -maxdepth 1 -type f -name "*.vcd" -exec cp {} "$new_out_dir" \;
else
    echo "Error: Input path '$in_path' is not a file or directory."
    exit 1
fi

