#!/usr/bin/env bash

debug=${1:-0}   # default 0, 1=debug

# --- check environment ---
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

if [ -d "$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results" ]; then
    rm -rf "$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results"
fi

# --- CSV list of parameter sets ---
# Each group is: nant,ncon,nspec,overlap

#Representative parameter sets for testing
param_list="2,2,10,0"
test_type="representative"

#Increasing spec numbers
#param_list="2,2,1,0 2,2,2,0 2,2,3,0 2,2,4,0 2,2,5,0 2,2,6,0 2,2,7,0 2,2,8,0 2,2,9,0 2,2,10,0"
#test_type="increasing_spec"

#Increasing overlap
#param_list="5,5,20,0 5,5,20,1 5,5,20,2 5,5,20,3 5,5,20,4 5,5,20,5 5,5,20,6 5,5,20,7 5,5,20,8 5,5,20,9 5,5,20,10"
#test_type="increasing_overlap"

#Incresing complexity
#param_list="1,1,10,0 2,1,10,0 2,2,10,0 3,2,10,0 3,3,10,0 4,3,10,0 4,4,10,0 5,4,10,0 5,5,10,0"
#test_type="increasing_complexity"

next_results_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results/Tests/"
elapsed_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results/Elapsed/"
other_stats_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results/Other_Stats/"

##remove the And_Results directory if it exists

mkdir -p "$next_results_dir"
mkdir -p "$elapsed_dir"
mkdir -p "$other_stats_dir"

#if summary_report.csv does not exist, create it and add header
if [ ! -f "$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results/${test_type}_and_summary_report.csv" ]; then
    echo "Experiment,Nant,Ncon,Nspec,Overlap,Time,Design_Lines" > "$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results/${test_type}_and_summary_report.csv"
fi

# --- loop through each parameter set ---
for param in $param_list; do
    IFS=',' read -r nant ncon nspec overlap <<< "$param"

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap"

    # build the formula string dynamically
    formula="{formula : G(..&&.. |-> ..&&..), nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"


    out_dir="$next_results_dir/And_A${nant}_C${ncon}_S${nspec}_O${overlap}"

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

    mv "$out_dir/.elapsed.csv" "$elapsed_dir/And_A${nant}_C${ncon}_S${nspec}_O${overlap}_elapsed.csv"

    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/input_generator/count_design_lines.sh" "$out_dir" > "$other_stats_dir/And_A${nant}_C${ncon}_S${nspec}_O${overlap}_design_size.txt"

    # extract elapsed time from the .elapsed.csv file, second line after the comma
    elapsed_time=$(sed -n '2p' "$elapsed_dir/And_A${nant}_C${ncon}_S${nspec}_O${overlap}_elapsed.csv" | cut -d',' -f2)
    design_lines=$(cat "$other_stats_dir/And_A${nant}_C${ncon}_S${nspec}_O${overlap}_design_size.txt")
    # append results to summary_report.csv
    echo "And,${nant},${ncon},${nspec},${overlap},${elapsed_time},${design_lines}" >> "$USMT_ROOT/tool/synthetic_gen/raw_challenges/And_Results/${test_type}_and_summary_report.csv"


done

