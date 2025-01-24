#!/bin/bash

# Check if the input and output paths are provided
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input_csv_directory> <output_pathectory>"
  exit 1
fi

input_path=$1
output_path=$2

if [ ! -d "$input_path" ]; then
    echo "Error: Input path '$input_path' is not directory."
    exit 1
fi

if [ ! -d "$output_path" ]; then
    echo "Error: Output '$output_path' is not a directory."
    exit 1
fi


process_csv() {
  local input_file=$1
  {
    # Read the header line and extract the last word of each item
    IFS=',' read -r -a header <<< "$(head -n 1 "$input_file")"
    for (( i=0; i<${#header[@]}; i++ )); do
      header[$i]="${header[$i]##* }"  # Extract the last word after the last space
    done

    # Process the remaining lines
    tail -n +2 "$input_file" | while IFS=',' read -r -a values; do
      for (( i=0; i<${#header[@]}; i++ )); do
        echo "x$i = ${values[$i]}"
      done
      echo ".."
    done
  }
}

concatenated_file="$output_path/all_traces.txt"

# Concatenate all processed .csv files in the directory
{
  for csv_file in "$input_path"/*.csv; do
    if [ -f "$csv_file" ]; then
      process_csv "$csv_file"
      echo "--"  # Separator between files
    fi
  done
} >> "$concatenated_file"

# if exists in the output directory, remove it
if [ -f "$output_path/$VARIABLES_MAP_FILE" ]; then
  rm "$output_path/$VARIABLES_MAP_FILE"
fi

# Handle variable mapping dump backwards
for (( i=${#header[@]}; i>0; i-- )); do
  echo "${header[$i-1]},x$((i-1))" >> "$output_path/$VARIABLES_MAP_FILE"
done


