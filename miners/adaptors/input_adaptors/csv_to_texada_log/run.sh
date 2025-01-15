#!/bin/bash

# Check if the input and output paths are provided
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input_csv_file_or_directory> <output_directory>"
  exit 1
fi

input_path=$1
output_dir=$2

# Ensure the output path is a directory
if [ ! -d "$output_dir" ]; then
  echo "Error: Output path '$output_dir' is not a directory."
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

# Check if input is a file
if [ -f "$input_path" ]; then
  if [[ "$input_path" == *.csv ]]; then
    output_file="$output_dir/$(basename "$input_path" .csv).txt"
    process_csv "$input_path" > "$output_file"
  else
    echo "Error: Input file '$input_path' is not a .csv file."
    exit 1
  fi

# Check if input is a directory
elif [ -d "$input_path" ]; then
  base_name=$(basename "$input_path")
  concatenated_file="$output_dir/$base_name/all_traces.txt"
  mkdir -p "$output_dir/$base_name"

  # Concatenate all processed .csv files in the directory
  {
    for csv_file in "$input_path"/*.csv; do
      if [ -f "$csv_file" ]; then
        process_csv "$csv_file"
        echo "--"  # Separator between files
      fi
    done
  } >> "$concatenated_file"

else
  echo "Error: Input path '$input_path' is not a file or directory."
  exit 1
fi

# Handle variable mapping dump backwards
for (( i=${#header[@]}; i>0; i-- )); do
  echo "${header[$i-1]},x$((i-1))" >> "$output_dir/$VARIABLES_MAP_FILE"
done


