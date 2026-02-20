#!/bin/bash

debug=${1:-0}   # default 0, 1=debug

# --- check environment ---
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi


##===========AND===========

echo "===========AND=========="
param_list="1,1,10,0 2,1,10,0 2,2,10,0 3,2,10,0 3,3,10,0 4,3,10,0 4,4,10,0 5,4,10,0 5,5,10,0"



result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_complex_and_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Complexity,Time" > "$result_file"

for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"
    formula="{formula : G(..&&.. |-> ..&&..), nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"
    out_dir="/tmp/And_A${nant}_C${ncon}_S${nspec}_O${overlap}"
    rm -rf "$out_dir"

    # run the experiment
    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/wrapper.sh" \
        "$out_dir" \
        And \
        And_top \
        clk \
        "$formula" \
        rst \
        And_top_bench::And_top_ \
        10000 \
        0 \
        "$debug" \

    complexity=$((nant + ncon))

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$complexity,$elapsed_time" >> "$result_file"
done

echo "Results written to $result_file"

#===========Until===========

echo "===========UNTIL=========="
param_list="1,1,10,0 2,1,10,0 2,2,10,0 3,2,10,0 3,3,10,0 4,3,10,0 4,4,10,0 5,4,10,0 5,5,10,0"



result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_complex_until_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Complexity,Time" > "$result_file"

for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"
    tmp_formula="G(..&&.. |-> ..&&.. U ..&&..)"
    #if ncon is equal to 1:
    if [ "$ncon" -eq 1 ]; then
        tmp_formula="G(..&&.. |-> ..&&..)"
    fi

    # build the formula string dynamically
    formula="{formula : $tmp_formula , nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"

    out_dir="/tmp/Until_A${nant}_C${ncon}_S${nspec}_O${overlap}"
    rm -rf "$out_dir"

    # run the experiment
    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/wrapper.sh" \
        "$out_dir" \
        Until \
        Until_top \
        clk \
        "$formula" \
        rst \
        Until_top_bench::Until_top_ \
        10000 \
        0 \
        "$debug" \

    complexity=$((nant + ncon))

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$complexity,$elapsed_time" >> "$result_file"

done

#===========Eventually===========

echo "===========EVENTUALLY=========="
param_list="1,1,10,0 2,1,10,0 2,2,10,0 3,2,10,0 3,3,10,0 4,3,10,0 4,4,10,0 5,4,10,0 5,5,10,0"


result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_complex_eventually_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Complexity,Time" > "$result_file"

for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"

    # build the formula string dynamically
    formula="{formula : G(..&&.. |-> F ..&&..), nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"

    out_dir="/tmp/Eventually_A${nant}_C${ncon}_S${nspec}_O${overlap}"
    rm -rf "$out_dir"

    # run the experiment
    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/wrapper.sh" \
        "$out_dir" \
        Eventually \
        Eventually_top \
        clk \
        "$formula" \
        rst \
        Eventually_top_bench::Eventually_top_ \
        10000 \
        0 \
        "$debug" \

    complexity=$((nant + ncon))
    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$complexity,$elapsed_time" >> "$result_file"

done
