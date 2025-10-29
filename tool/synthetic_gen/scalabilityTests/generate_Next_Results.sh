#!/usr/bin/env bash

debug=${1:-0}   # default 0, 1=debug

# --- check environment ---
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

if [ -d "$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results" ]; then
    rm -rf "$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results"
fi

# --- CSV list of parameter sets ---
# Each group is: nant,ncon,nspec,overlap
#param_list="2,2,10,0, G(..##1.. |-> ..##1..); \"
#test_type="representative"

#Incresing depth
param_list="1,1,10,0,G(..##1.. |-> ..##1..); \
1,2,10,0,G(..##1.. |-> ..##1..); \
1,2,10,0,G(..##1.. |-> ..##2..); \
1,2,10,0,G(..##1.. |-> ..##3..); \
1,2,10,0,G(..##1.. |-> ..##4..); \
1,2,10,0,G(..##1.. |-> ..##5..); \
1,2,10,0,G(..##1.. |-> ..##6..); \
1,2,10,0,G(..##1.. |-> ..##7..); \
1,2,10,0,G(..##1.. |-> ..##8..)"
test_type="increasing_depth"

#param_list="2,2,1,0,G(..##1.. |-> ..##1..); \
#2,2,1,0,G(..##1.. |-> ..##1..); \
#2,2,2,0,G(..##1.. |-> ..##1..); \
#2,2,3,0,G(..##1.. |-> ..##1..); \
#2,2,4,0,G(..##1.. |-> ..##1..); \
#2,2,5,0,G(..##1.. |-> ..##1..); \
#2,2,6,0,G(..##1.. |-> ..##1..); \
#2,2,7,0,G(..##1.. |-> ..##1..); \
#2,2,8,0,G(..##1.. |-> ..##1..); \
#2,2,9,0,G(..##1.. |-> ..##1..); \
#2,2,10,0,G(..##1.. |-> ..##1..)"
#test_type="increasing_spec"

next_results_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results/Tests/"
elapsed_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results/Elapsed/"
other_stats_dir="$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results/Other_Stats/"

#remove the Next_Results directory if it exists

mkdir -p "$next_results_dir"
mkdir -p "$elapsed_dir"
mkdir -p "$other_stats_dir"

#if summary_report.csv does not exist, create it and add header
if [ ! -f "$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results/${test_type}_next_summary_report.csv" ]; then
    echo "Experiment,Nant,Ncon,Nspec,Overlap,Time,Design_Lines,Depth" > "$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results/${test_type}_next_summary_report.csv"
fi

# --- loop through each parameter set ---
IFS=';' read -ra params <<< "$param_list"
for param in "${params[@]}"; do
    param=$(echo "$param" | xargs)  # trim whitespace
    IFS=',' read -r nant ncon nspec overlap formula <<< "$param"

    N=$(echo "$formula" | sed -n 's/[^>]*##\([0-9]\+\).*/\1/p')
    M=$(echo "$formula" | sed -n 's/.*##\([0-9]\+\).*/\1/p')

    # calculate depth
    depth=$(((N * (nant - 1)) + (M * (ncon - 1))))

    echo "Running experiment with nant=$nant ncon=$ncon nspec=$nspec overlap=$overlap formula=$formula depth=$depth"

    # build the formula string dynamically
    config="{formula : $formula, nant : $nant, ncon : $ncon, nspec: $nspec, overlap : $overlap}"



    out_dir="$next_results_dir/Next_A${nant}_C${ncon}_S${nspec}_O${overlap}_D${depth}/"

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

    mv "$out_dir/.elapsed.csv" "$elapsed_dir/Next_A${nant}_C${ncon}_S${nspec}_O${overlap}_D${depth}_elapsed.csv"

    bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/input_generator/count_design_lines.sh" "$out_dir" > "$other_stats_dir/Next_A${nant}_C${ncon}_S${nspec}_O${overlap}_D${depth}_design_size.txt"

    # extract elapsed time from the .elapsed.csv file, second line after the comma
    elapsed_time=$(sed -n '2p' "$elapsed_dir/Next_A${nant}_C${ncon}_S${nspec}_O${overlap}_D${depth}_elapsed.csv" | cut -d',' -f2)
    design_lines=$(cat "$other_stats_dir/Next_A${nant}_C${ncon}_S${nspec}_O${overlap}_D${depth}_design_size.txt")
    # append results to summary_report.csv
    echo "Next,${nant},${ncon},${nspec},${overlap},${elapsed_time},${design_lines},${depth}" >> "$USMT_ROOT/tool/synthetic_gen/raw_challenges/Next_Results/${test_type}_next_summary_report.csv"


done

