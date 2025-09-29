#!/bin/bash

#default values
debug="${1:-0}"

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

python3 "$USMT_ROOT/tool/synthetic_gen/testGenerator/input_generator/input_generator.py" \
--parallel 1 \
--clk clk \
--top_module sample_ \
--debug $debug \
--templates "{G(..&&.. |=> ..&&..),2,2,3,0}" \
--tracelength 1000 \
--outdir work
