#!/bin/bash

# Check input arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <formulas_file> <target_formula> <output_file>"
    exit 1
fi

FORMULAS_FILE="$1"
TARGET_FORMULA="$2"
OUTPUT_FILE="$3"

# Read all formulas from the file into an array
mapfile -t FORMULAS < "$FORMULAS_FILE"


result=""

while [[ 1 ]]; do

if [ ${#FORMULAS[@]} -eq 0 ]; then
  echo "Formula set is empty"
  break
#else 
#  echo "Current set of formulas: ${FORMULAS[@]}"
fi

# Create the initial concatenated formula
FORMULA_SET=$(printf " && %s" "${FORMULAS[@]}")
FORMULA_SET=${FORMULA_SET:4} # Remove leading " && "
FORMULA_SET+=" && ~($TARGET_FORMULA)"
ALL_FORMULA_SET=$(printf " && %s" "${FORMULAS[@]}")
ALL_FORMULA_SET=${ALL_FORMULA_SET:4}
ALL_FORMULA_SET="$ALL_FORMULA_SET && !($TARGET_FORMULA)"
#echo "All formulas: $ALL_FORMULA_SET"
# Test if the entire set implies the target formula
#check if FORMULAS is empty

RESULT=$(../third_party/spot/bin/ltl2tgba --formula "$ALL_FORMULA_SET" | tail -n2 | head -n1)


if [[ "$RESULT" != "State: 0" ]]; then
  echo "The entire set does not imply the target formula"
  break
fi  

echo "-------------------------------------------------"


rm -f tmp.txt
  

# Start testing removals
for ((i = 0; i < ${#FORMULAS[@]}; i++)); do

    TEST_FORMULAS=("${FORMULAS[@]:0:i}" "${FORMULAS[@]:i+1}")
    TEST_FORMULA_SET=$(printf " && %s" "${TEST_FORMULAS[@]}")
    TEST_FORMULA_SET=${TEST_FORMULA_SET:4}
    TEST_FORMULA_SET="$TEST_FORMULA_SET && !($TARGET_FORMULA)"

    # Run the command with the test formula
    RESULT=$(../third_party/spot/bin/ltl2tgba --formula "$TEST_FORMULA_SET" | tail -n2 | head -n1)

    if [[ "$RESULT" == "State: 0" ]]; then
        #append the removed formula to tmp.txt
        echo "${FORMULAS[i]}" >> tmp.txt
        #echo "Appended ${FORMULAS[i]} to tmp.txt"
        #adjust i
        i=$((i-1))
        FORMULAS=("${TEST_FORMULAS[@]}")
    else
        echo "Could not remove formula: ${FORMULAS[i]}"
        result+=" ${FORMULAS[i]}"
    fi

done


unset FORMULAS

mapfile -t FORMULAS < "tmp.txt"
#delete last element if it is empty
if [ -z "${FORMULAS[-1]}" ]; then
    unset 'FORMULAS[${#FORMULAS[@]}-1]'
fi

done

# Save the remaining formulas to the output file
printf "Result is: %s\n" "$result"

