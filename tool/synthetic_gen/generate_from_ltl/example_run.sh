#!/bin/bash

#default values
debug="${1:-0}"

python generate_from_ltl/generate_from_ltl.py \
--parallel 1 \
--clk clk \
--top_module top_mod \
--debug $debug \
--templates "{G(..&&.. |=> ..&&..),3,3,5,0}" \
--tracelength 1000 \
--outdir work
