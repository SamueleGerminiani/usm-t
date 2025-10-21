#!/usr/bin/env bash

# Usage: ./count_verilog_lines.sh /path/to/search

# Exit if no argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

DIR="$1"

# Check if the provided path is a directory
if [ ! -d "$DIR" ]; then
    echo "Error: '$DIR' is not a valid directory."
    exit 1
fi

# Find all .v files and count total lines
total_lines=$(find "$DIR" -type f -name "*.v" -exec cat {} + | wc -l)

echo "$total_lines"

