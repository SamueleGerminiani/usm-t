#!/bin/bash

if [ "$#" -ne 10 ]; then
    echo "Usage: $0 <output_folder> <test_name> <top_module_name> <clock_name> <template_string> <reset_signal> <vcd_scope> <tracelenght> <install> <debug>"
    exit 1
fi

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

TEST_OUTPUT_FOLDER=$1
TEST_NAME=$2
TOP_MODULE_NAME=$3
CLOCK_NAME=$4
TEMPLATE_STRING=$5
RESET_NAME=$6
VCD_SCOPE=$7
TRACE_LENGTH=$8
INSTALL="${9:-0}"
DEBUG="${10:-0}"

CONFIG_OUTPUT_FOLDER="$TEST_OUTPUT_FOLDER/test_config"
INPUT_GENERATOR_OUTPUT_FOLDER="$TEST_OUTPUT_FOLDER/test_input"
OUT_TEST_FILE="${TEST_OUTPUT_FOLDER}/test_config/${TEST_NAME}_synthetic.xml"

##Uninstall test
if [ "$INSTALL" -eq 2 ]; then
    #rm configurations to USMT
    rm -rf "$USMT_ROOT/miners/tools/harm/configurations/${TEST_NAME}"
    rm -rf "$USMT_ROOT/miners/tools/goldminer/configurations/${TEST_NAME}"
    rm -rf "$USMT_ROOT/miners/tools/texada/configurations/${TEST_NAME}"
    rm -rf "$USMT_ROOT/miners/tools/samples2ltl/configurations/${TEST_NAME}"


    #rm the input
    rm -rf "$USMT_ROOT/input/${TEST_NAME}"

    #rm the test file
    rm -f "$USMT_ROOT/tests/${TEST_NAME}_synthetic.xml"
    
    echo "Test uninstallation completed successfully."
    exit 0
fi


# Check if TEST_OUTPUT_FOLDER exists and delete if it does
if [ -d "$TEST_OUTPUT_FOLDER" ]; then
    echo "Deleting existing syntgen_folder: $TEST_OUTPUT_FOLDER"
    rm -rf "$TEST_OUTPUT_FOLDER"
fi

mkdir -p "$TEST_OUTPUT_FOLDER"


# Generate the design using input_generator.py
echo "Generating test input (design + traces) using input_generator.py..."
INPUT_GENERATOR_OUTPUT_STRING=$(
    python3 "$USMT_ROOT/tool/synthetic_gen/testGenerator/input_generator/input_generator.py" \
        --parallel "1" \
        --top_module "$TOP_MODULE_NAME" \
        --clk "$CLOCK_NAME" \
        --outdir "$INPUT_GENERATOR_OUTPUT_FOLDER" \
        --templates "$TEMPLATE_STRING" \
        --tracelength "$TRACE_LENGTH" \
        --debug "$DEBUG" \
    | tee /dev/tty | tail -n 1
)

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate input."
    exit 1
fi

# Extract returned string from input_generator.py
HINTS=$(echo "$INPUT_GENERATOR_OUTPUT_STRING" | tail -n 1)  # Assuming the last line is the output string

# Run miner_config_generator.sh
echo "Generating sample miner configurations with miner_config_generator.sh..."
$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/miner_config_generator.sh "$CONFIG_OUTPUT_FOLDER" "$TEST_NAME" "$TOP_MODULE_NAME" "$CLOCK_NAME" "$RESET_NAME" "$VCD_SCOPE" "$HINTS" "$INSTALL"

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate test."
    exit 1
fi

echo "Test generation completed successfully."


harm_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/harm/configurations/${TEST_NAME}"
goldminer_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/goldminer/configurations/${TEST_NAME}"
texada_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/texada/configurations/${TEST_NAME}"
samples2ltl_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/samples2ltl/configurations/${TEST_NAME}"
gemini_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/gemini-flash/configurations/${TEST_NAME}"

#Install test
if [ "$INSTALL" -eq 1 ]; then
    #cp configurations to USMT
    cp -r "$harm_config_dir" "$USMT_ROOT/miners/tools/harm/configurations/"
    cp -r "$goldminer_config_dir" "$USMT_ROOT/miners/tools/goldminer/configurations/"
    cp -r "$texada_config_dir" "$USMT_ROOT/miners/tools/texada/configurations/"
    cp -r "$samples2ltl_config_dir" "$USMT_ROOT/miners/tools/samples2ltl/configurations/"
    cp -r "$gemini_config_dir" "$USMT_ROOT/miners/tools/gemini-flash/configurations/"

    #cp the input
    cp -r "$INPUT_GENERATOR_OUTPUT_FOLDER" "$USMT_ROOT/input/${TEST_NAME}"

    #cp the test file
    cp "$OUT_TEST_FILE" "$USMT_ROOT/tests/"

    echo "Test installation completed successfully."
fi




