#!/bin/bash

# Usage: ./generate_tests.sh <syntgen_folder> <config_folder> <output_folder> <test_name> <clock_name> <reset_signal> <vcd_scope>

if [ "$#" -ne 10 ]; then
    echo "Usage: $0 <syntgen_folder> <output_folder> <test_name> <top_module_name> <clock_name> <template_string> <reset_signal> <vcd_scope> <tracelenght> <install>"
    exit 1
fi

SYNTGEN_FOLDER=$1
CONFIG_FOLDER="$USMT_ROOT/miners/usmt_default_config.xml"
OUTPUT_FOLDER=$2
TEST_NAME=$3
TOP_MODULE_NAME=$4
CLOCK_NAME=$5
TEMPLATE_STRING=$6
RESET_SIGNAL=$7
VCD_SCOPE=$8
TRACELENGTH=$9
INSTALL="${10:-0}"


# Check if SYNTGEN_FOLDER exists and delete if it does
if [ -d "$SYNTGEN_FOLDER" ]; then
    echo "Deleting existing syntgen_folder: $SYNTGEN_FOLDER"
    rm -rf "$SYNTGEN_FOLDER"
fi

# Check if OUTPUT_FOLDER exists and delete if it does
if [ -d "$OUTPUT_FOLDER" ]; then
    echo "Deleting existing output_folder: $OUTPUT_FOLDER"
    rm -rf "$OUTPUT_FOLDER"
fi

# Generate the design using generate_from_ltl.py
echo "Generating design using generate_from_ltl.py..."
GENERATE_OUTPUT=$(python3 $USMT_ROOT/tool/synthetic_gen/generate_from_ltl/generate_from_ltl.py "--parallel" "1" "--top_module" "$TOP_MODULE_NAME" "--clk" "$CLOCK_NAME" "--outdir" "$SYNTGEN_FOLDER" "--templates" "$TEMPLATE_STRING" "--tracelength" "$TRACELENGTH")
#python3 $USMT_ROOT/tool/synthetic_gen/generate_from_ltl/generate_from_ltl.py "--parallel" "1" "--top_module" "$TOP_MODULE_NAME" "--clk" "$CLOCK_NAME" "--outdir" "$SYNTGEN_FOLDER" "--templates" "$TEMPLATE_STRING" "--tracelength" "$TRACELENGTH" "--debug" "1"

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate design."
    exit 1
fi

# Extract returned string from generate_from_ltl.py
RETURNED_STRING=$(echo "$GENERATE_OUTPUT" | tail -n 1)  # Assuming the last line is the output string

# Run generateTest.sh
echo "Running generateTest.sh..."
$USMT_ROOT/tool/synthetic_gen/testGeneration/generateTest.sh "$SYNTGEN_FOLDER" "$CONFIG_FOLDER" "$OUTPUT_FOLDER" "$TEST_NAME" "$TOP_MODULE_NAME" "$CLOCK_NAME" "$RESET_SIGNAL" "$VCD_SCOPE" "$RETURNED_STRING" "$INSTALL"

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate test."
    exit 1
fi

echo "Test generation completed successfully."
