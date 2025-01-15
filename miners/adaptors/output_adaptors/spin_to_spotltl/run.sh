#!/bin/bash

# Function to display usage
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

#check that sed is installed
if ! command -v sed &> /dev/null
then
    echo "sed could not be found"
    exit
fi

# Input and output paths
in_path=$1
out_path=$2
substitutions_file="$3"

# Ensure the input file exists
if [ ! -f "$in_path" ]; then
  echo "Error: Input file '$in_path' not found."
  exit 1
fi

# Ensure the output_path points to a directory
if [ -f "$out_path" ]; then
  echo "Error: Output path '$out_path' is a file, not a directory."
  exit 1
fi

# Hardcoded replacements
declare -A replacements=(
  ["\s"]=""  # Remove all spaces
  ["\\[\\]"]="G"  # Escaped square brackets
  ["\([a-zA-Z_][a-zA-Z0-9_]*\)=0"]="!\1"      
  ["=1"]="" #convert =1 to boolean      
  ["\([a-zA-Z_][a-zA-Z0-9_]*\)=0"]="!\1" #convert =0 to !boolean      
)

file_name=$(basename $in_path)
out_path=$out_path/$file_name

# Create a copy of the input file as the output file
cp "$in_path" "$out_path"
#retreive the file name

# Apply replacements using sed
for original in "${!replacements[@]}"; do
  replacement=${replacements[$original]}
  sed -i "s/$original/$replacement/g" "$out_path"
done


# Read the substitutions file line by line
while IFS=, read -r to from
do
    # Use sed to replace the text in the output file
    sed -i "s/$from/$to/g" "$out_path"
done < "$substitutions_file"

