#!/usr/bin/env bash

debug=${1:-0}   # default 0, 1=debug

# --- check environment ---
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

if [ -d "$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results" ]; then
    rm -rf "$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results"
fi

# --- CSV list of parameter sets ---
# Each group is: nant,ncon,nspec,overlap
param_list="3,3,10,0"
test_type="representative"

next_results_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results/Tests/"
elapsed_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results/Elapsed/"
other_stats_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results/Other_Stats/"

##remove the NextAnd_Results directory if it exists

mkdir -p "$next_results_dir"
mkdir -p "$elapsed_dir"
mkdir -p "$other_stats_dir"

#if summary_report.csv does not exist, create it and add header
if [ ! -f "$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results/${test_type}_next_and_summary_report.csv" ]; then
    echo "Experiment,Nant,Ncon,Nspec,Overlap,Time,Design_Lines" > "$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results/${test_type}_next_and_summary_report.csv"
fi

# --- loop through each parameter set ---
for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"

    # build the formula string dynamically
    formula="{formula : G(..#1&.. |-> ..#1&..), nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"


    out_dir="$next_results_dir/NextAnd_A${nant}_C${ncon}_S${nspec}_O${overlap}"

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

    mv "$out_dir/.elapsed.csv" "$elapsed_dir/NextAnd_A${nant}_C${ncon}_S${nspec}_O${overlap}_elapsed.csv"

    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/input_generator/count_design_lines.sh" "$out_dir" > "$other_stats_dir/NextAnd_A${nant}_C${ncon}_S${nspec}_O${overlap}_design_size.txt"

    # extract elapsed time from the .elapsed.csv file, second line after the comma
    elapsed_time=$(sed -n '2p' "$elapsed_dir/NextAnd_A${nant}_C${ncon}_S${nspec}_O${overlap}_elapsed.csv" | cut -d',' -f2)
    design_lines=$(cat "$other_stats_dir/NextAnd_A${nant}_C${ncon}_S${nspec}_O${overlap}_design_size.txt")
    # append results to summary_report.csv
    echo "NextAnd,${nant},${ncon},${nspec},${overlap},${elapsed_time},${design_lines}" >> "$USMT_ROOT/tool/synthetic_gen/raw_challenges/NextAnd_Results/${test_type}_next_and_summary_report.csv"


done

