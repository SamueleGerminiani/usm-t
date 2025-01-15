#!/bin/bash

# Check if the correct number of arguments was provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <input_file> <output_directory> <substitutions_file>"
    exit 1
fi

input_file="$1"
output_directory="$2"
substitutions_file="$3"

# Make sure the input file and substitutions file exist
if [ ! -f "$substitutions_file" ] || [ ! -f "$input_file" ]; then
    echo "Error: Both files must exist."
    exit 1
fi

# Make sure the output directory exists or create it
if [ ! -d "$output_directory" ]; then
  echo "Error: Output directory does not exist or is not a directory."
  exit 1
fi

# Extract the base name of the input file and prepare the output path
file_name=$(basename "$input_file")
out_path="$output_directory/$file_name"
cp "$input_file" "$out_path"

# Read the substitutions file line by line
while IFS=, read -r to from
do
    # Use sed to replace the text in the output file
    sed -i "s/$from/$to/g" "$out_path"
done < "$substitutions_file"

