#!/bin/bash
function generate_texada_run() {
    local output_file=$1
    local template=$2

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    echo "./texada -m --parse-mult-prop -f "$template" --log-file /input/\$LOG_FILE --out-file /output/\$MINED_SPECIFICATIONS_FILE ; cat /output/\$MINED_SPECIFICATIONS_FILE" >&3

    # Close file descriptor
    exec 3>&-
}
