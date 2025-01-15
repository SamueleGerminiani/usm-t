#!/bin/bash

# Function to display usage
usage() {
  echo "Usage: $0 <in_path> <out_path>"
  echo "  in_path: Path to the directory with the assertions, ex. goldmine.out/arb2/verif/prism/"
  echo "  out_path: Path to the output text file"
  exit 1
}


# Ensure correct number of arguments
if [ "$#" -lt 2 ]; then
  usage
fi


#check that sed is installed
if ! command -v sed &> /dev/null
then
    echo "sed could not be found"
    exit 1
fi

#check that grep is installed
if ! command -v grep &> /dev/null
then
    echo "grep could not be found"
    exit 1
fi

# Input and output paths
in_path=$1
out_path=$2

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
  ["\([a-zA-Z_][a-zA-Z0-9_]*\)==0"]="!\1" #convert =0 to !boolean      
  ["==1"]="" #convert =1 to boolean      
  ["|->"]="}|->" 
  ["|=>"]="}|->" 
  ["&"]=" && " #convert & to &&
)

file_name=$(basename $in_path)
out_path=$out_path/$file_name
 
#for each file in the list, extract the lines that start with "a\d: " and write them to the output file
grep -Po '^a\d: \K.*' $in_path > $out_path

# Apply replacements using sed
for original in "${!replacements[@]}"; do
  replacement=${replacements[$original]}
  sed -i "s/$original/$replacement/g" "$out_path"
done

#Add 'G(' in front of all lines of the out_put file and add ')' at the end of all lines
sed -i 's/^/G({/; s/$/)/' "$out_path"




