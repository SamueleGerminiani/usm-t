#!/bin/bash

# Check if the correct number of arguments was provided
usage() {
  echo "Usage: $0 <in_path> <out_path>"
  echo " in_path: Path to the input text file"
  echo " out_path: Path to the output text file"
  echo " var_map: Path to the variable map file"
  exit 1
}

# Ensure the correct number of arguments are provided
if [ "$#" -ne 3 ]; then
  usage
fi

# Input and output paths
in_path="$1"
out_path="$2"
substitutions_file="$3"

# Ensure the input file exists
if [ ! -f "$in_path" ]; then
  echo "Error: Input file '$in_path' not found."
  exit 1
fi

# Ensure the substitutions file exists
if [ ! -f "$substitutions_file" ]; then
  echo "Error: Substitutions file '$substitutions_file' not found."
  exit 1
fi

cp "$in_path" "$out_path"

# Read the substitutions file line by line
while IFS=, read -r to from
do
    # Use sed to replace the text in the output file
    sed -i "s/$from/$to/g" "$out_path"
done < "$substitutions_file"
