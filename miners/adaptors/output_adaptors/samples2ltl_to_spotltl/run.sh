#!/bin/bash

# Check if the correct number of arguments was provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <input_file> <output_file> <substitutions_file>"
    exit 1
fi

input_file="$1"
out_path="$2"
substitutions_file="$3"

# Make sure the input file and substitutions file exist
if [ ! -f "$substitutions_file" ] || [ ! -f "$input_file" ]; then
    echo "Error: Both files must exist."
    exit 1
fi

cp "$input_file" "$out_path"

# Read the substitutions file line by line
while IFS=, read -r to from
do
    # Use sed to replace the text in the output file
    sed -i "s/$from/$to/g" "$out_path"
done < "$substitutions_file"

