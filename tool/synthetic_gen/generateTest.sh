#!/bin/bash
#!/bin/bash

function generate_harm_xml() {
    local input_string=$1
    local output_file=$2
    local template="$3"

    # Parse the input string into antecedent and consequence variables
    local ant=$(echo $input_string | grep -oP '"ant":"\K[^"]*')
    local con=$(echo $input_string | grep -oP '"con":"\K[^"]*')

    # Split the antecedent and consequence variables into arrays
    IFS=',' read -ra ant_vars <<< "$ant"
    IFS=',' read -ra con_vars <<< "$con"

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    # Start the XML output
    echo "<harm>" >&3
    echo "  <context name=\"default\">" >&3

    # Generate XML entries for antecedent variables
    for var in "${ant_vars[@]}"; do
        echo "    <prop exp=\"$var\" loc=\"dt\"/>" >&3
    done

    # Generate XML entries for consequence variables
    for var in "${con_vars[@]}"; do
        echo "    <prop exp=\"$var\" loc=\"c\"/>" >&3
    done

    # Add the remaining static parts of the XML
    echo "    <template dtLimits=\"3A,1E,O\" exp=\"$template\" />" >&3
    echo "    <sort name=\"causality\" exp=\"1-afct/traceLength\"/>" >&3
    echo "    <sort name=\"frequency\" exp=\"atct/traceLength\"/>" >&3
    echo "  </context>" >&3
    echo "</harm>" >&3

    # Close file descriptor
    exec 3>&-
}

function generate_harm_run() {
    local output_file=$1

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    # Start the XML output
    echo "./harm --clk clock --conf /input/config.xml --vcd /input/$golden_vcd_name --dump-to /output/\$MINED_SPECIFICATIONS_FILE --vcd-ss $vcd_scope --max-threads 1" >&3

    # Close file descriptor
    exec 3>&-
}


function generate_goldminer_config() {
    local max_sim_cycles=$1
    local num_cycles=$2
    local num_propositions=$3
    local output_file=$4

    # Open file descriptor to write output to the specified file
    exec 3> "$output_file"

    # Print the configuration text with the substituted values
    echo "# GoldMine Configuration File" >&3
    echo "" >&3
    echo "# Root directory of VCS installation" >&3
    echo "vcs_home::/path/to/VCS/Root/Directory" >&3
    echo "# Synopsys License string" >&3
    echo "synopsys_license::<license_string>" >&3
    echo "" >&3
    echo "# Root directory of IFV installation" >&3
    echo "ifv_root::/path/to/IFV/Root/Directory" >&3
    echo "# Cadence License string" >&3
    echo "cadence_license::<license_string>" >&3
    echo "" >&3
    echo "# Root directory of IVerilog executable" >&3
    echo "iverilog_home::/usr/bin/" >&3
    echo "" >&3
    echo "# Mining Engine (Valid Options: dtree, bgdf, prism)" >&3
    echo "engine::prism" >&3
    echo "" >&3
    echo "# Maximum number of simulation cycles" >&3
    echo "max_sim_cycles::$max_sim_cycles" >&3
    echo "# Maximum temporal length of an assertion" >&3
    echo "num_cycles::$num_cycles" >&3
    echo "# Maximum number of propositions in the antecedent of an assertion" >&3
    echo "num_propositions::$num_propositions" >&3
    echo "# Maximum number of counter-examples to be used to refine an assertion" >&3
    echo "num_counterexample::0" >&3
    echo "" >&3
    echo "# Decision Forest Parameters" >&3
    echo "num_partitions::+" >&3
    echo "" >&3
    echo "# Coverage Miner Parameters" >&3
    echo "min_coverage::0.01" >&3

    # Close file descriptor
    exec 3>&-
}

function generate_goldminer_run() {
    local output_file=$1

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    echo "python2.7 ../src/goldmine.py -m test -u /input/ -S -V -f /input/ --clock \"$clk:1\" -v /input/$golden_vcd_name ; find goldmine.out/ -name \"*.gold\" -exec cat {} + > /output/\$MINED_SPECIFICATIONS_FILE" >&3

    # Close file descriptor
    exec 3>&-
}

function generate_texada_run() {
    local output_file=$1
    local template=$2

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    echo "./texada -m --parse-mult-prop -f "$template" --log-file /input/\$LOG_FILE --out-file /output/\$MINED_SPECIFICATIONS_FILE ; cat /output/\$MINED_SPECIFICATIONS_FILE" >&3

    # Close file descriptor
    exec 3>&-
}

function generate_samples2ltl_run() {
    local output_file=$1
    local max_depth=$2


    # Open file descriptor to write output to file
    exec 3> "$output_file"

    echo "python3 samples2ltl.py --test_sat_method --start_depth 3 --max_num_formulas 1000 --timeout 1800 --max_depth $max_depth --force_always_implication 1 --traces /input/\$LOG_FILE --max_trace_length \$MAX_TRACE_LENGTH --dump_to /output/\$MINED_SPECIFICATIONS_FILE && cat /output/\$MINED_SPECIFICATIONS_FILE" >&3

    # Close file descriptor
    exec 3>&-
}



# Check for necessary program(s)
if ! command -v sed &> /dev/null
then
    echo "sed could not be found, please install sed to continue."
    exit 1
fi

default_config_file="$1"
out_dir_path="$2"
test_name="$3"
hints="$4"
clk="$5"
rst="$6"
top_module="$7"
vcd_scope="$8"
verilog_dir="$9"
golden_vcd_file="${10}"
golden_vcd_name=$(basename $golden_vcd_file)
golden_csv_file="${11}"
golden_csv_name=$(basename $golden_csv_file)
faulty_vcd_dir="${12}"
expected_file="${13}"
template="${14}"
out_file="$out_dir_path/${test_name}_test.xml"

# Check for correct number of arguments
if [ "$#" -ne 14 ]; then
    echo "Usage: $0 <default_config_file> <out_dir_path> <test_name> <hints> <clk> <rst> <top_module> <vcd_scope> <verilog_dir> <golden_vcd_file> <golden_csv_file> <faulty_vcd_dir> <expected_file> <template>"
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
["<GOLDMINER_CONFIGURATION>"]="${test_name}_config/config.cfg"
["<GOLDMINER_RUN>"]="${test_name}_config/run_miner.sh"
)

# Perform substitutions
cp "$default_config_file" "$out_file"

# Perform substitutions using sed with alternative delimiter "|"
for original in "${!substitutions[@]}"
do
    sub=${substitutions[$original]}
    sed -i "s|$original|$sub|g" "$out_file"
done

#HARM
mkdir -p "${out_dir_path}/tools/harm/configurations/${test_name}"
harm_config_file_path="${out_dir_path}/tools/harm/configurations/${test_name}/config.xml"
generate_harm_xml "$hints" "$harm_config_file_path" "$template"
harm_run_file_path="${out_dir_path}/tools/harm/configurations/${test_name}/run_miner.sh"
generate_harm_run "$harm_run_file_path"

#GoldMiner
mkdir -p "${out_dir_path}/tools/goldminer/configurations/${test_name}"
goldminer_config_file_path="${out_dir_path}/tools/goldminer/configurations/${test_name}/goldmine.cfg"
generate_goldminer_config "1000" "4" "3" "$goldminer_config_file_path"
goldminer_run_file_path="${out_dir_path}/tools/goldminer/configurations/${test_name}/run_miner.sh"
generate_goldminer_run "$goldminer_run_file_path"

#Texada
mkdir -p "${out_dir_path}/tools/texada/configurations/${test_name}"
texada_run_file_path="${out_dir_path}/tools/texada/configurations/${test_name}/run_miner.sh"
generate_texada_run "$texada_run_file_path" "$template"

#Samples2LTL
mkdir -p "${out_dir_path}/tools/samples2ltl/configurations/${test_name}"
samples2ltl_run_file_path="${out_dir_path}/tools/samples2ltl/configurations/${test_name}/run_miner.sh"
generate_samples2ltl_run "$samples2ltl_run_file_path" "4"

echo "Substitutions completed. Output is in '$out_file'."

