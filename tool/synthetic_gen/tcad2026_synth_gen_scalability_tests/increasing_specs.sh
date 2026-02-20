#!/bin/bash

debug=${1:-0}   # default 0, 1=debug

# --- check environment ---
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi


#===========AND===========

echo "===========AND=========="
param_list="2,2,10,0 2,2,20,0 2,2,30,0 2,2,40,0 2,2,50,0 2,2,60,0 2,2,70,0 2,2,80,0 2,2,90,0 2,2,100,0"



result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_spec_and_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Nspec,Time" > "$result_file"

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

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$nspec,$elapsed_time" >> "$result_file"
done

echo "Results written to $result_file"

#===========NEXT===========

echo "===========NEXT=========="

param_list="2,2,10,0,G(..##1.. |-> ..##1..); \
2,2,20,0,G(..##1.. |-> ..##1..); \
2,2,30,0,G(..##1.. |-> ..##1..); \
2,2,40,0,G(..##1.. |-> ..##1..); \
2,2,50,0,G(..##1.. |-> ..##1..); \
2,2,60,0,G(..##1.. |-> ..##1..); \
2,2,70,0,G(..##1.. |-> ..##1..); \
2,2,80,0,G(..##1.. |-> ..##1..); \
2,2,90,0,G(..##1.. |-> ..##1..); \
2,2,100,0,G(..##1.. |-> ..##1..)"

result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_spec_next_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Nspec,Time" > "$result_file"

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

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$nspec,$elapsed_time" >> "$result_file"

done

#===========NEXT_AND===========
echo "===========NEXT_AND=========="
param_list="3,3,10,0 3,3,20,0 3,3,30,0 3,3,40,0 3,3,50,0 3,3,60,0 3,3,70,0 3,3,80,0 3,3,90,0 3,3,100,0"
result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_spec_nextAnd_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Nspec,Time" > "$result_file"

for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"

    # build the formula string dynamically
    formula="{formula : G(..#1&.. |-> ..#1&..), nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"


    out_dir="/tmp/NextAnd_A${nant}_C${ncon}_S${nspec}_O${overlap}"
    rm -rf "$out_dir"

    # run the experiment
    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/wrapper.sh" \
        "$out_dir" \
        NextAnd \
        NextAnd_top \
        clk \
        "$formula" \
        rst \
        NextAnd_top_bench::NextAnd_top_ \
        10000 \
        0 \
        "$debug" \

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$nspec,$elapsed_time" >> "$result_file"

done

#===========Until===========

#echo "===========UNTIL=========="
param_list="2,2,10,0 2,2,20,0 2,2,30,0 2,2,40,0 2,2,50,0 2,2,60,0 2,2,70,0 2,2,80,0 2,2,90,0 2,2,100,0"



result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_spec_until_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Nspec,Time" > "$result_file"

for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"

    # build the formula string dynamically
    formula="{formula : G(..&&.. |-> ..&&.. U ..&&..), nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"

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

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$nspec,$elapsed_time" >> "$result_file"

done

#===========Eventually===========

echo "===========EVENTUALLY=========="
param_list="2,2,10,0 2,2,20,0 2,2,30,0 2,2,40,0 2,2,50,0 2,2,60,0 2,2,70,0 2,2,80,0 2,2,90,0 2,2,100,0"


result_file="$USMT_ROOT/tool/synthetic_gen/tcad2026_synth_gen_scalability_tests/results/increasing_spec_eventually_chart.csv"
rm -f "$result_file"
#make header of result file
echo "Nspec,Time" > "$result_file"

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

    elapsed_time=$(sed -n '2p' "$out_dir/.elapsed.csv" | cut -d',' -f2)

    echo "$nspec,$elapsed_time" >> "$result_file"

done
