#!/bin/bash
#
function getMaxDepth()
{
  local N=$1
  local length=$2

  local result=$((N * (length - 1)))

  echo "$result"
}

function generate_gemini_agent_setup(){

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi
dump_path=$1
local hint_string=$2
# Parse the input string into antecedent and consequence variables
local ant=$(echo "$hint_string" | grep -oP '{ant\s*:\s*\K[^;]*')
local con=$(echo "$hint_string" | grep -oP '\scon\s*:\s*\K[^;]*')
local formula=$(echo "$hint_string" | grep -oP 'formula\s*:\s*\K[^;}]*')
formula="${formula// /}"  # remove spaces
local nant=$(echo "$hint_string" | grep -oP 'nant\s*:\s*\K[0-9]+')
local ncon=$(echo "$hint_string" | grep -oP 'ncon\s*:\s*\K[0-9]+')
local nspec=$(echo "$hint_string" | grep -oP 'nspec\s*:\s*\K[0-9]+')
imp_sign=$(echo "$formula" | grep -oP '(->|=>|\|->|\|=>)')

N=$(echo "$formula" | sed -n 's/[^>]*##\?\([0-9]\+\).*/\1/p')
M=$(echo "$formula" | sed -n 's/.*##\?\([0-9]\+\).*/\1/p')

#echo "N: $N"
#echo "M: $M"
#echo "nant: $nant"
#echo "ncon: $ncon"

if [ -n "$N" ]; then
    N_max_depth=$(getMaxDepth $N $nant)
fi
if [ -n "$M" ]; then
    M_max_depth=$(getMaxDepth $M $ncon)
fi

echo "N_max_depth: $N_max_depth"
echo "M_max_depth: $M_max_depth"

tot_max_depth=$((N_max_depth + M_max_depth))

if [[ "$imp_sign" == "=>" || "$imp_sign" == "|=>" ]]; then
  tot_max_depth=$((tot_max_depth + 1))
fi


# Perform substitutions using sed with alternative delimiter "|"
agent_setup_path="$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/agent_instructions.txt"

cp "$agent_setup_path" "$dump_path"


declare -A gen_subs_map=(
["<DEPTH_NOTE>"]="The temporal depth of the specifications is between 0 and $tot_max_depth"
["<IN_VARS>"]="$ant"
["<OUT_VARS>"]="$con"
)
echo $ant
echo $con

for original in "${!gen_subs_map[@]}"
do
    sub=${gen_subs_map[$original]}
    sed -i "s|$original|$sub|g" "$dump_path"
done

}



function generate_gemini_run() {
    local output_file=$1

    gemini_run="input_files=\$(ls /input/*.v | paste -sd, -)\\n"
    gemini_run=$gemini_run"python3 runAgent.py --input \$input_files --agent-setup /input/\$AGENT_INSTRUCTIONS --dump-to /output/query_out.txt --api-key \$GEMINI_API_KEY\\n"
    gemini_run=$gemini_run"sed -n '/\[Start\]/,/\[End\]/p' /output/query_out.txt | sed '1d;\$d' > /output/\$MINED_SPECIFICATIONS_FILE\\n"
    gemini_run=$gemini_run"cat /output/\$MINED_SPECIFICATIONS_FILE\\n"

    #dump to file
    echo -e "$gemini_run" > "$output_file"
}


#generate_gemini_agent_setup "$1" "$2"
#cat "$1"
