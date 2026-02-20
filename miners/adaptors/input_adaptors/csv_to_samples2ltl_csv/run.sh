#!/bin/bash

# Check if the input and output paths are provided
if [ "$#" -lt 2 ]; then
  echo "(Adaptor) Usage: $0 <input_csv_directory> <output_directory>"
  exit 1
fi

input_path=$1
output_path=$2

if [ ! -d "$input_path" ]; then
    echo "(Adaptor) Error: Input path '$input_path' is not a file or directory."
    exit 1
fi

if [ ! -d "$output_path" ]; then
    echo "(Adaptor) Error: Output '$output_path' is not a directory."
    exit 1
fi

process_csv() {
  local input_file=$1
  local output_file=$2

  {
    # Read the header line and extract the last word of each item
    IFS=',' read -r -a header <<< "$(head -n 1 "$input_file")"
    for (( i=0; i<${#header[@]}; i++ )); do
      header[$i]="${header[$i]##* }"  # Extract the last word after the last space
    done

    # Process all data lines (skipping the header)
    tail -n +2 "$input_file" | while IFS=',' read -r -a values; do
      echo "$(IFS=,; echo "${values[*]}")" # Join data elements with commas
    done | paste -sd ";" -  # Join lines with semicolon
  } >> "$output_file"
}

concatenated_file="$output_path/all_traces.txt"
#echo "Concatenating all processed .csv files in '$input_path' to '$concatenated_file'..."

# Concatenate all processed .csv files in the directory
{
  for csv_file in "$input_path"/*.csv; do
    if [ -f "$csv_file" ]; then
      process_csv "$csv_file" "$concatenated_file"
    fi
  done
}

# Handle variable mapping dump backwards
# Note: This relies on the 'header' array being set by the last run of process_csv
for (( i=${#header[@]}; i>0; i-- )); do
  echo "${header[$i-1]},x$((i-1))" >> "$output_path/$VARIABLES_MAP_FILE"
done
