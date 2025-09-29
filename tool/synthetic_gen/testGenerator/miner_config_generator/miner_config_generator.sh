#!/bin/bash

#check that USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "USMT_ROOT is not set. Please set USMT_ROOT to the root directory of the USMT repository."
    exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/generateHARM.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateHARM.sh"
  exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/generateTexada.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateTexada.sh"
  exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/generateGoldminer.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateGoldminer.sh"
  exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/generateSamples2ltl.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateSamples2ltl.sh"
  exit 1
fi

#Add new miner here...


# Check for necessary program(s)
if ! command -v sed &> /dev/null
then
    echo "sed could not be found, please install sed to continue."
    exit 1
fi

#This is the path to the directory containing a generated synthetic design and traces (must follow a predifined internal structure and naming convention) + the output path for the config files
CONFIG_OUTPUT_FOLDER="$1"
#Path to the template of the test file: this will be used to generate the test file
TEST_CONFIG_TEMPLATE="$USMT_ROOT/miners/usmt_default_config.xml"
#Name of the test (arbitrary)
TEST_NAME="$2"
#Top module of the input design
TOP_MODULE_NAME="$3"
#Clock signal of the input design
CLOCK_NAME="$4"
#Reset signal of the input design
RESET_NAME="$5"
#Scope of the VCD file top::module1::module2:: ... ::modulen
VCD_SCOPE="$6"
#Python-like dictionary containing HINTS to help generating the configuration files, ex. '{"ant":"a_0,a_1","con":"b_0,b_1"}' specifies which variables go in the antecedent and consequent of a template
HINTS="$7"


input_base_name=$(basename $CONFIG_OUTPUT_FOLDER)
verilog_dir="$input_base_name/design/"
golden_vcd_file="$input_base_name/traces/vcd/golden.vcd"
golden_vcd_name=$(basename $golden_vcd_file)
golden_csv_file="$input_base_name/traces/csv/golden.csv"
golden_csv_name=$(basename $golden_csv_file)
faulty_vcd_dir="input/$input_base_name/faulty_traces/vcd/"
expected_file="input/$input_base_name/expected/specifications.txt"
out_test_file="$CONFIG_OUTPUT_FOLDER/${TEST_NAME}_synthetic.xml"

#check that the generated_design path exists and is a directory
if [ ! -d "$CONFIG_OUTPUT_FOLDER" ]; then
  mkdir -p "$CONFIG_OUTPUT_FOLDER"
fi
#check that the TEST_CONFIG_TEMPLATE exists and is a file
if [ ! -f "$TEST_CONFIG_TEMPLATE" ]; then
    echo "The TEST_CONFIG_TEMPLATE does not exist or is not a file."
    exit 1
fi

#check if output directory already exists
if [ ! -d "$CONFIG_OUTPUT_FOLDER" ]; then
    mkdir -p "$CONFIG_OUTPUT_FOLDER"
fi


# Check that we have at least 7 arguments
if [ "$#" -lt 7 ]; then
    echo "Usage: $0 <CONFIG_OUTPUT_FOLDER> <TEST_NAME> <TOP_MODULE_NAME> <CLOCK_NAME> <RESET_NAME> <VCD_SCOPE> <HINTS>" 
    exit 1
fi


# Define substitutions: "ORIGINAL":"SUB"
declare -A substitutions=(
["<GOLDEN_VCD_FILE>"]="$golden_vcd_file"
["<CLK>"]="$CLOCK_NAME"
["<RST>"]="$RESET_NAME"
["<VCD_SCOPE>"]="$VCD_SCOPE"
["<VERILOG_DIR>"]="$verilog_dir"
["<CSV_FILE>"]="$golden_csv_file"
["<TOP_MODULE>"]="$TOP_MODULE_NAME"
["<TEST_NAME>"]="$TEST_NAME"
["<EXPECTED_FILE>"]="$expected_file"
["<FAULTY_VCD_DIR>"]="$faulty_vcd_dir"
["<HARM_CONFIGURATION>"]="${TEST_NAME}/config.xml"
["<HARM_RUN>"]="${TEST_NAME}/run_miner.sh"
["<TEXADA_RUN>"]="${TEST_NAME}/run_miner.sh"
["<SAMPLES2LTL_RUN>"]="${TEST_NAME}/run_miner.sh"
["<GOLDMINER_CONFIGURATION>"]="${TEST_NAME}/goldmine.cfg"
["<GOLDMINER_RUN>"]="${TEST_NAME}/run_miner.sh"
)

# Perform substitutions
cp "$TEST_CONFIG_TEMPLATE" "$out_test_file"

# Perform substitutions using sed with alternative delimiter "|"
for original in "${!substitutions[@]}"
do
    sub=${substitutions[$original]}
    sed -i "s|$original|$sub|g" "$out_test_file"
done

#HARM
harm_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/harm/configurations/${TEST_NAME}"
mkdir -p "${harm_config_dir}"
harm_config_file_path="${harm_config_dir}/config.xml"
generate_harm_xml "$HINTS" "$harm_config_file_path" "G(..#1&.. |-> P0)"
harm_run_file_path="${harm_config_dir}/run_miner.sh"
generate_harm_run "$harm_run_file_path" "$CLOCK_NAME"

#Goldminer
goldminer_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/goldminer/configurations/${TEST_NAME}"
mkdir -p "${goldminer_config_dir}"
goldminer_config_file_path="${goldminer_config_dir}/goldmine.cfg"
generate_goldminer_config "1000" "4" "3" "$goldminer_config_file_path"
goldminer_run_file_path="${goldminer_config_dir}/run_miner.sh"
generate_goldminer_run "$goldminer_run_file_path" "$TOP_MODULE_NAME"

#Texada
texada_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/texada/configurations/${TEST_NAME}"
mkdir -p "${texada_config_dir}"
texada_run_file_path="${texada_config_dir}/run_miner.sh"
generate_texada_run "$texada_run_file_path" "[](P0 -> P1)"

#Samples2LTL
samples2ltl_config_dir="${CONFIG_OUTPUT_FOLDER}/tools/samples2ltl/configurations/${TEST_NAME}"
mkdir -p "${samples2ltl_config_dir}"
samples2ltl_run_file_path="${samples2ltl_config_dir}/run_miner.sh"
generate_samples2ltl_run "$samples2ltl_run_file_path" "4"

#Add new miner here... (extend the install logic in the wrapper if needed)


