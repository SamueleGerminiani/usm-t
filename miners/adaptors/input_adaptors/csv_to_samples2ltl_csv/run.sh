#!/bin/bash

# Check if the input and output paths are provided
if [ "$#" -lt 2 ]; then
  echo "Usage: $0 <input_csv_file_or_directory> <output_directory> [max_length]"
  exit 1
fi

input_path=$1
output_dir=$2
max_length=${3:-100}  # Default to 50 if max_length is not provided, indicating no row cut


if [ ! -d "$output_dir" ]; then
  echo "Error: Output path '$output_dir' is not a directory."
  exit 1
fi

process_csv() {
  local input_file=$1
  local output_file=$2
  local max_lines=$3

  {
    # Read the header line and extract the last word of each item
    IFS=',' read -r -a header <<< "$(head -n 1 "$input_file")"
    for (( i=0; i<${#header[@]}; i++ )); do
      header[$i]="${header[$i]##* }"  # Extract the last word after the last space
    done

    # Process the data lines
    if [ "$max_lines" -gt 0 ]; then
      # Process only up to max_length lines if specified
      tail -n +2 "$input_file" | head -n "$max_lines"
    else
      # Process all lines if max_length is not specified
      tail -n +2 "$input_file"
    fi | while IFS=',' read -r -a values; do
      echo "$(IFS=,; echo "${values[*]}")" # Join data elements with commas
    done | paste -sd ";" -  # Join lines with semicolon
  } >> "$output_file"
}

# Check if input is a file
if [ -f "$input_path" ]; then
  if [[ "$input_path" == *.csv ]]; then
    output_file="$output_dir/$(basename "$input_path" .csv).txt"
    process_csv "$input_path" "$output_file" "$max_length"
  else
    echo "Error: Input file '$input_path' is not a .csv file."
    exit 1
  fi

# Check if input is a directory
elif [ -d "$input_path" ]; then
  base_name=$(basename "$input_path")
  concatenated_file="$output_dir/$base_name/all_traces.txt"
  mkdir -p "$output_dir/$base_name"
  echo "Concatenating all processed .csv files in '$input_path' to '$concatenated_file'..."

  # Concatenate all processed .csv files in the directory
  {
    for csv_file in "$input_path"/*.csv; do
      if [ -f "$csv_file" ]; then
        process_csv "$csv_file" "$concatenated_file" "$max_length"
      fi
    done
  }

else
  echo "Error: Input path '$input_path' is not a file or directory."
  exit 1
fi

# if exists in the output directory, remove it
if [ -f "$output_dir/$VARIABLES_MAP_FILE" ]; then
  rm "$output_dir/$VARIABLES_MAP_FILE"
fi

# Handle variable mapping dump backwards
for (( i=${#header[@]}; i>0; i-- )); do
  echo "${header[$i-1]},x$((i-1))" >> "$output_dir/$VARIABLES_MAP_FILE"
done


