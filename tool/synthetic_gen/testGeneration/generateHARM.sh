#!/bin/bash
function generate_harm_xml() {
    local hint_string=$1
    local output_file=$2
    local template="$3"

    # Parse the input string into antecedent and consequence variables
    local ant=$(echo $hint_string | grep -oP '"ant":"\K[^"]*')
    local con=$(echo $hint_string | grep -oP '"con":"\K[^"]*')

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
    echo "./harm --clk clock --conf /input/config.xml --vcd /input/$golden_vcd_name --dump-to /output/\$MINED_SPECIFICATIONS_FILE --vcd-ss $vcd_scope --max-threads \$MAX_THREADS" >&3

    # Close file descriptor
    exec 3>&-
}


