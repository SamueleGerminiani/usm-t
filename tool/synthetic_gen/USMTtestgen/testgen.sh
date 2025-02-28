#!/bin/bash

# Usage: ./generate_tests.sh <syntgen_folder> <config_folder> <output_folder> <test_name> <clock_name> <reset_signal> <vcd_scope>

if [ "$#" -ne 8 ]; then
    echo "Usage: $0 <syntgen_folder> <output_folder> <test_name> <top_module_name> <clock_name> <reset_signal> <vcd_scope> <tracelenght>"
    exit 1
fi

SYNTGEN_FOLDER=$1
#CONFIG_FOLDER=$2
CONFIG_FOLDER="$USMT_ROOT/miners/usmt_default_config.xml"
OUTPUT_FOLDER=$2
TEST_NAME=$3
TOP_MODULE_NAME=$4
CLOCK_NAME=$5
RESET_SIGNAL=$6
VCD_SCOPE=$7
TRACELENGHT=$8

# Generate the design using generate_from_ltl.py
#python src/generate_from_ltl.py --parallel 1 --clk clk --debug 0 --templates "{G(..&&.. |=> F ..&&..),3,2,2,3}" --tracelenght 10000
echo "Generating design using generate_from_ltl.py..."
GENERATE_OUTPUT=$(python3 $USMT_ROOT/tool/synthetic_gen/src/generate_from_ltl.py "--parallel" "1" "--top_module" "$TOP_MODULE_NAME" "--clk" "$CLOCK_NAME" "--outdir" "$SYNTGEN_FOLDER" "--templates" "\"{G(..&&.. |=> F ..&&..),3,2,2,3}\"" "--tracelenght" "$TRACELENGHT")

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate design."
    exit 1
fi

# Extract returned string from generate_from_ltl.py
RETURNED_STRING=$(echo "$GENERATE_OUTPUT" | tail -n 1)  # Assuming the last line is the output string

# Determine the top module from the design folder
TOP_MODULE="top_module"  # Adjust if top module is defined differently

# Run generateTest.sh
echo "Running generateTest.sh..."
$USMT_ROOT/tool/synthetic_gen/testGeneration/generateTest.sh "$SYNTGEN_FOLDER" "$CONFIG_FOLDER" "$OUTPUT_FOLDER" "$TEST_NAME" "$TEST_BASENAME" "$TOP_MODULE" "$CLOCK_NAME" "$RESET_SIGNAL" "$VCD_SCOPE" "$RETURNED_STRING" "0" 

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate test."
    exit 1
fi

echo "Test generation completed successfully."