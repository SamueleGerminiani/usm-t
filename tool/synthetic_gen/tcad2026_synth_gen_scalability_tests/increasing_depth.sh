#!/bin/bash

debug=${1:-0}   # default 0, 1=debug

# --- check environment ---
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi


#===========AND===========

#===========NEXT===========

echo "===========NEXT=========="

param_list="1,1,10,0,G(..##1.. |-> ..##1..); \
1,2,10,0,G(..##1.. |-> ..##1..); \
1,2,10,0,G(..##1.. |-> ..##2..); \
1,2,10,0,G(..##1.. |-> ..##3..); \
1,2,10,0,G(..##1.. |-> ..##4..); \
1,2,10,0,G(..##1.. |-> ..##5..); \
1,2,10,0,G(..##1.. |-> ..##6..); \
1,2,10,0,G(..##1.. |-> ..##7..); \
1,2,10,0,G(..##1.. |-> ..##8..)"

result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_depth_next_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Depth,Time,NLines" > "$result_file"

IFS=';' read -ra params <<< "$param_list"
for param in "${params[@]}"; do
    param=$(echo "$param" | xargs)  # trim whitespace
    IFS=',' read -r nant ncon nspec overlap formula <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"

    out_dir="/tmp/Next_A${nant}_C${ncon}_S${nspec}_O${overlap}"
    rm -rf "$out_dir"

    # build the formula string dynamically
    config="{formula : $formula, nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"

    # run the experiment
    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/wrapper.sh" \
        "$out_dir" \
        Next \
        Next_top \
        clk \
        "$config" \
        rst \
        Next_top_bench::Next_top_ \
        10000 \
        0 \
        "$debug" \

    N=$(echo "$formula" | sed -n 's/[^>]*##\([0-9]\+\).*/\1/p')
    M=$(echo "$formula" | sed -n 's/.*##\([0-9]\+\).*/\1/p')

    # calculate depth
    depth=$(((N * (nant - 1)) + (M * (ncon - 1))))

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/input_generator/count_design_lines.sh" "$out_dir" > "$out_dir/.design_size.txt"

    design_lines=$(cat "$out_dir/.design_size.txt")


    echo "$depth,$elapsed_time,$design_lines" >> "$result_file"

done

