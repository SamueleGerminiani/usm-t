#!/bin/bash
function generate_samples2ltl_run() {
    local output_file=$1
    local max_depth=$2


    # Open file descriptor to write output to file
    exec 3> "$output_file"

    echo "python3 samples2ltl.py --test_sat_method --start_depth 3 --max_num_formulas 1000 --timeout 1800 --max_depth $max_depth --force_always_implication 1 --traces /input/\$LOG_FILE --max_trace_length \$MAX_TRACE_LENGTH --dump_to /output/\$MINED_SPECIFICATIONS_FILE && cat /output/\$MINED_SPECIFICATIONS_FILE" >&3

    # Close file descriptor
    exec 3>&-
}
