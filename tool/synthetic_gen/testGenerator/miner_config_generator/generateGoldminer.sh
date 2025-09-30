#!/bin/bash
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
    local top_module=$2
    local clk=$3
    local golden_vcd_name=$4

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    echo "python2.7 ../src/goldmine.py -m $top_module -u /input/ -S -V -f /input/ --clock \"$clk:1\" -v /input/$golden_vcd_name ; find goldmine.out/ -name \"*.gold\" -exec cat {} + > /output/\$MINED_SPECIFICATIONS_FILE" >&3

    # Close file descriptor
    exec 3>&-
}


