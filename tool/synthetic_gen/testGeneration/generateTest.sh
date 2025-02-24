#!/bin/bash

#check that USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "USMT_ROOT is not set. Please set USMT_ROOT to the root directory of the USMT repository."
    exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGeneration/generateHARM.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateHARM.sh"
  exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGeneration/generateTexada.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateTexada.sh"
  exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGeneration/generateGoldminer.sh"
if [ $? -ne 0 ]; then
  echo "Failed to source generateGoldminer.sh"
  exit 1
fi

source "$USMT_ROOT/tool/synthetic_gen/testGeneration/generateSamples2ltl.sh"
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

#This is the path to the directory containing a generated synthetic design (must follow a predifined internal structure and naming convention)
generated_design_path="$1"
#Path to the template of the test file: this will be used to generate the test file
default_config_file="$2"
#Path to the output directory where all the files will be generated
out_dir_path="$3"
#Name of the test (arbitrary)
test_name="$4"
#Top module of the input design
top_module="$5"
#Clock signal of the input design
clk="$6"
#Reset signal of the input design
rst="$7"
#Scope of the VCD file top::module1::module2:: ... ::modulen
vcd_scope="$8"
#Python-like dictionary containing hints to help generating the configuration files, ex. '{"ant":"a_0,a_1","con":"b_0,b_1"}' specifies which variables go in the antecedent and consequent of a template
hints="$9"
#optional: defaults to 0 which does nothing, 1 copies the files in the correct location of usmt, 2 deletes the files from those locations
install_test="${10:-0}"


input_base_name=$(basename $generated_design_path)
verilog_dir="$input_base_name/design/"
golden_vcd_file="$input_base_name/traces/vcd/golden.vcd"
golden_vcd_name=$(basename $golden_vcd_file)
golden_csv_file="$input_base_name/traces/csv/golden.csv"
golden_csv_name=$(basename $golden_csv_file)
faulty_vcd_dir="$input_base_name/faulty_traces/vcd/"
expected_file="$input_base_name/expected/specifications.txt"
out_test_file="$out_dir_path/${test_name}_synthetic.xml"

#check that the generated_design path exists and is a directory
if [ ! -d "$generated_design_path" ]; then
    echo "The generated_design path does not exist or is not a directory."
    exit 1
fi
#check that the default_config_file exists and is a file
if [ ! -f "$default_config_file" ]; then
    echo "The default_config_file does not exist or is not a file."
    exit 1
fi

#check if output directory already exists
if [ -d "$out_dir_path" ]; then
    echo "Output directory already exists. Please specify a different output directory."
    exit 1
fi

mkdir -p "$out_dir_path"

# Check that we have at least 9 arguments
if [ "$#" -lt 9 ]; then
    echo "Usage: $0 <default_config_file> <out_dir_path> <test_name> <input_base_name> <top_module> <clk> <rst> <vcd_scope> <hints>" 
    exit 1
fi


# Define substitutions: "ORIGINAL":"SUB"
declare -A substitutions=(
["<GOLDEN_VCD_FILE>"]="$golden_vcd_file"
["<CLK>"]="$clk"
["<RST>"]="$rst"
["<VCD_SCOPE>"]="$vcd_scope"
["<VERILOG_DIR>"]="$verilog_dir"
["<CSV_FILE>"]="$golden_csv_file"
["<TOP_MODULE>"]="$top_module"
["<TEST_NAME>"]="$test_name"
["<EXPECTED_FILE>"]="$expected_file"
["<FAULTY_VCD_DIR>"]="$faulty_vcd_dir"
["<HARM_CONFIGURATION>"]="${test_name}_config/config.xml"
["<HARM_RUN>"]="${test_name}_config/run_miner.sh"
["<TEXADA_RUN>"]="${test_name}_config/run_miner.sh"
["<SAMPLES2LTL_RUN>"]="${test_name}_config/run_miner.sh"
["<GOLDMINER_CONFIGURATION>"]="${test_name}_config/goldmine.cfg"
["<GOLDMINER_RUN>"]="${test_name}_config/run_miner.sh"
)

# Perform substitutions
cp "$default_config_file" "$out_test_file"

# Perform substitutions using sed with alternative delimiter "|"
for original in "${!substitutions[@]}"
do
    sub=${substitutions[$original]}
    sed -i "s|$original|$sub|g" "$out_test_file"
done

#HARM
harm_config_dir="${out_dir_path}/tools/harm/configurations/${test_name}"
mkdir -p "${harm_config_dir}"
harm_config_file_path="${harm_config_dir}/config.xml"
generate_harm_xml "$hints" "$harm_config_file_path" "G(..#1&.. |-> P0)"
harm_run_file_path="${harm_config_dir}/run_miner.sh"
generate_harm_run "$harm_run_file_path"

#Goldminer
goldminer_config_dir="${out_dir_path}/tools/goldminer/configurations/${test_name}"
mkdir -p "${goldminer_config_dir}"
goldminer_config_file_path="${goldminer_config_dir}/goldmine.cfg"
generate_goldminer_config "1000" "4" "3" "$goldminer_config_file_path"
goldminer_run_file_path="${goldminer_config_dir}/run_miner.sh"
generate_goldminer_run "$goldminer_run_file_path"

#Texada
texada_config_dir="${out_dir_path}/tools/texada/configurations/${test_name}"
mkdir -p "${texada_config_dir}"
texada_run_file_path="${texada_config_dir}/run_miner.sh"
generate_texada_run "$texada_run_file_path" "[](P0 -> P1)"

#Samples2LTL
samples2ltl_config_dir="${out_dir_path}/tools/samples2ltl/configurations/${test_name}"
mkdir -p "${samples2ltl_config_dir}"
samples2ltl_run_file_path="${samples2ltl_config_dir}/run_miner.sh"
generate_samples2ltl_run "$samples2ltl_run_file_path" "4"

#Add new miner here... (fix the install logic)


#Install test
if [ "$install_test" -eq 1 ]; then
    #cp configurations to USMT
    cp -r "$harm_config_dir" "$USMT_ROOT/miners/tools/harm/configurations/"
    cp -r "$goldminer_config_dir" "$USMT_ROOT/miners/tools/goldminer/configurations/"
    cp -r "$texada_config_dir" "$USMT_ROOT/miners/tools/texada/configurations/"
    cp -r "$samples2ltl_config_dir" "$USMT_ROOT/miners/tools/samples2ltl/configurations/"

    #cp the input
    cp -r "$generated_design_path" "$USMT_ROOT/input/"

    #cp the test file
    cp "$out_test_file" "$USMT_ROOT/tests/"

fi

#Uninstall test
if [ "$install_test" -eq 2 ]; then
    #rm configurations to USMT
    rm -rf "$USMT_ROOT/miners/tools/harm/configurations/${test_name}"
    rm -rf "$USMT_ROOT/miners/tools/goldminer/configurations/${test_name}"
    rm -rf "$USMT_ROOT/miners/tools/texada/configurations/${test_name}"
    rm -rf "$USMT_ROOT/miners/tools/samples2ltl/configurations/${test_name}"

    #rm the input
    rm -rf "$USMT_ROOT/input/${input_base_name}"

    #rm the test file
    rm "$USMT_ROOT/tests/$(basename $out_test_file)"
fi
