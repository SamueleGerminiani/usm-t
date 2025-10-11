#!/bin/bash

function extendTexadaRun() {
  local new_formula=$1
  #add ; if texada_run is not empty
  if [ -n "$texada_run" ]; then
    texada_run=$texada_run"\\n" 
  else
    texada_run="touch /output/.temp\\n"
  fi
  #texada_run=$texada_run"./texada -m --parse-mult-prop -f "\"$new_formula\"" --log-file /input/\$LOG_FILE --out-file /output/\$MINED_SPECIFICATIONS_FILE ; cat /output/\$MINED_SPECIFICATIONS_FILE \\n"
  texada_run=$texada_run"echo \"--------------------------------------------\"\\n"
  texada_run=$texada_run"echo \"Running with template $new_formula\"\\n"
  texada_run=$texada_run"echo \"--------------------------------------------\"\\n"
  texada_run=$texada_run"timeout --foreground 120s ./texada -m --parse-mult-prop -f "\"$new_formula\"" --log-file /input/\$LOG_FILE --out-file /output/.temp \\n"
  #append .temp to MINED_SPECIFICATIONS_FILE
  texada_run=$texada_run"cat /output/.temp >> /output/\$MINED_SPECIFICATIONS_FILE \\n"
  texada_run=$texada_run"cat /output/.temp\\n"
    
}

function makeAnd() {
  local start_placeholder_index=$1
  local length=$2
  local result="P${start_placeholder_index}"
  for (( i=1; i<length; i++ )); do
    result="${result} && P$((start_placeholder_index + i))"
  done
  #add brackets if length > 1
  if (( length > 1 )); then
    result="(${result})"
  fi

  #accumulate for debug
  echo "$result"
}

function makeShift()
{
  local N=$1
  local length=$2
  #make a sequence Xs according to N*length
  local result=""
  for (( i=0; i<length; i++ )); do
    for (( j=0; j<N; j++ )); do
      result="${result}X"
    done
  done

  echo "$result"
}

function makeNextChain() {
  local N=$1
  local start_placeholder_index=$2
  local length=$3
  local result="P${start_placeholder_index}"
  for (( i=1; i<length; i++ )); do
    result="${result} && $(makeShift $N $i)P$((start_placeholder_index + i))"
  done
  #add brackets if length > 1
  if (( length > 1 )); then
    result="(${result})"
  fi

  #accumulate for debug
  echo "$result"
}
#add a number of Xs depending on the input N value and the number of placeholders



function generate_texada_run() {
    local output_file=$1
    local hint_string=$2
    # Parse the input string into antecedent and consequence variables
    local ant=$(echo "$hint_string" | grep -oP '{ant\s*:\s*\K[^;}]*')
    local con=$(echo "$hint_string" | grep -oP '\scon\s*:\s*\K[^;}]*')
    local formula=$(echo "$hint_string" | grep -oP 'formula\s*:\s*\K[^;}]*')
    formula="${formula// /}"  # remove spaces
    local nant=$(echo "$hint_string" | grep -oP 'nant\s*:\s*\K[0-9]+')
    local ncon=$(echo "$hint_string" | grep -oP 'ncon\s*:\s*\K[0-9]+')
    local nspec=$(echo "$hint_string" | grep -oP 'nspec\s*:\s*\K[0-9]+')
    #DEBUG
    #echo "ant: $ant"
    #echo "con: $con"
    #echo "formula: $formula"
    #echo "nant: $nant"
    #echo "ncon: $ncon"
    #echo "nspec: $nspec"
    

    #need to handle the different template cases
    #G(..&&.. -> ..&&..)
    #G(..##N.. -> ..##M..)
    #G(..#N&.. -> ..#M&..)
    #G(..&&.. -> ..&&.. U ..&&..)
    #G(..&&.. -> F ..&&..)
    

    texada_run=""
    imp_sign=$(echo "$formula" | grep -oP '(->|=>|\|->|\|=>)')
    imp_shift=""

    #G(..&&.. -> ..&&..)
    if echo "$formula" | grep -Eq '^G\(..&&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..&&..\)$'; then
      #Extract the implication sign
      #if imp sign is => or |-> add a next
      if [[ "$imp_sign" == "=>" || "$imp_sign" == "|=>" ]]; then
        imp_shift="X"
      fi

      ant_1=$(makeAnd 0 1)
      ant_2=$(makeAnd 0 2)
      ant_3=$(makeAnd 0 3)

      con_1=$(makeAnd 10 1)
      con_2=$(makeAnd 10 2)
      con_3=$(makeAnd 10 3)

      template="[]($ant_1 -> ${imp_shift} $con_1)"
      extendTexadaRun "$template"

      #only if ncon > 1, add more templates
      if (( ncon > 1 )); then
          template="[]($ant_1 -> ${imp_shift} $con_2)"
          extendTexadaRun "$template"
          if (( nant > 1 )); then
            template="[]($ant_2 -> ${imp_shift} $con_1)"
            extendTexadaRun "$template"
            template="[]($ant_2 -> ${imp_shift} $con_2)"
            extendTexadaRun "$template"
          fi
      fi

      #Texada is too slow and generates too many specifications with more than 4 placeholders
      #if (( ncon > 2 )); then
      #  template="[]($ant_1 -> ${imp_shift} $con_3)"
      #  extendTexadaRun "$template"
      #  if (( nant > 1 )); then
      #    template="[]($ant_2 -> ${imp_shift} $con_3)"
      #    extendTexadaRun "$template"
      #    if (( nant > 2 )); then
      #      template="[]($ant_3 -> ${imp_shift} $con_1)"
      #      extendTexadaRun "$template"
      #      template="[]($ant_3 -> ${imp_shift} $con_2)"
      #      extendTexadaRun "$template"
      #      template="[]($ant_3 -> ${imp_shift} $con_3)"
      #      extendTexadaRun "$template"
      #    fi
      #  fi 
      #fi

    #G(..##N.. -> ..##M..)
    #G(..#N&.. -> ..#M&..)
  elif echo "$formula" | grep -Eq '^G\(..##?([0-9]+)&?..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..##?([0-9]+)&?..\)$'; then

      N=$(echo "$formula" | sed -n 's/[^>]*##\?\([0-9]\+\).*/\1/p')
      M=$(echo "$formula" | sed -n 's/.*##\?\([0-9]\+\).*/\1/p')

      #Extract the implication sign
      #if imp sign is => or |-> add a next
      if [[ "$imp_sign" == "=>" || "$imp_sign" == "|=>" ]]; then
        imp_shift="X"
      fi

      ant_1=$(makeNextChain $N 0 1)
      ant_2=$(makeNextChain $N 0 2)
      ant_3=$(makeNextChain $N 0 3)

      con_1=$(makeNextChain $M 10 1)
      con_2=$(makeNextChain $M 10 2)
      con_3=$(makeNextChain $M 10 3)

      template="[]($ant_1 -> ${imp_shift} $con_1)"
      extendTexadaRun "$template"

      #only if ncon > 1, add more templates
      if (( ncon > 1 )); then
          template="[]($ant_1 -> $(makeShift "$N" 0)${imp_shift} $con_2)"
          extendTexadaRun "$template"
          if (( nant > 1 )); then
            template="[]($ant_2 -> $(makeShift "$N" 1)${imp_shift} $con_1)"
            extendTexadaRun "$template"
            template="[]($ant_2 -> $(makeShift "$N" 1)${imp_shift} $con_2)"
            extendTexadaRun "$template"
          fi
      fi

      #Texada is too slow and generates too many specifications with more than 4 placeholders
      #
      #if (( ncon > 2 )); then
      #  template="[]($ant_1 -> $(makeShift "$N" 0)${imp_shift} $con_3)"
      #  extendTexadaRun "$template"
      #  if (( nant > 1 )); then
      #    template="[]($ant_2 -> $(makeShift "$N" 1)${imp_shift} $con_3)"
      #    extendTexadaRun "$template"
      #    if (( nant > 2 )); then
      #      template="[]($ant_3 -> $(makeShift "$N" 2)${imp_shift} $con_1)"
      #      extendTexadaRun "$template"
      #      template="[]($ant_3 -> $(makeShift "$N" 2)${imp_shift} $con_2)"
      #      extendTexadaRun "$template"
      #      template="[]($ant_3 -> $(makeShift "$N" 2)${imp_shift} $con_3)"
      #      extendTexadaRun "$template"
      #    fi
      #  fi 
      #fi

    #G(..&&.. -> ..&&.. U ..&&..)
    elif echo "$formula" | grep -Eq '^G\(..&&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..&&..[[:space:]]*U[[:space:]]*..&&..\)$'; then

      #Extract the implication sign
      #if imp sign is => or |-> add a next
      if [[ "$imp_sign" == "=>" || "$imp_sign" == "|=>" ]]; then
        imp_shift="X"
      fi

      ant_1=$(makeAnd 0 1)
      ant_2=$(makeAnd 0 2)

      con_1_1=$(makeAnd 10 1)
      con_1_2=$(makeAnd 11 1)
      con_2_1=$(makeAnd 10 2)
      con_2_2=$(makeAnd 12 2)

      template="[]($ant_1 -> ${imp_shift} ($con_1_1 U $con_1_2))"
      extendTexadaRun "$template"
      if (( nant > 1 )); then
        template="[]($ant_2 -> ${imp_shift} ($con_1_1 U $con_1_2))"
        extendTexadaRun "$template"
      fi

      if (( ncon > 1 )); then
        template="[]($ant_1 -> ${imp_shift} ($con_2_1 U $con_2_1))"
        extendTexadaRun "$template"
        if (( nant > 1 )); then
          template="[]($ant_2 -> ${imp_shift} ($con_2_1 U $con_2_2))"
          extendTexadaRun "$template"
        fi

      fi




    #G(..&&.. -> F ..&&..)
    elif echo "$formula" | grep -Eq '^G\(..&&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*F[[:space:]]*..&&..\)$'; then
      #Extract the implication sign
      #if imp sign is => or |-> add a next
      if [[ "$imp_sign" == "=>" || "$imp_sign" == "|=>" ]]; then
        imp_shift="X"
      fi

      ant_1=$(makeAnd 0 1)
      ant_2=$(makeAnd 0 2)

      con_1=$(makeAnd 10 1)
      con_2=$(makeAnd 10 2)

      template="[]($ant_1 -> ${imp_shift} <> $con_1)"
      extendTexadaRun "$template"

      #only if ncon > 1, add more templates
      if (( ncon > 1 )); then
          template="[]($ant_1 -> ${imp_shift} <> $con_2)"
          extendTexadaRun "$template"
          if (( nant > 1 )); then
            template="[]($ant_2 -> ${imp_shift} <> $con_1)"
            extendTexadaRun "$template"
            template="[]($ant_2 -> ${imp_shift} <> $con_2)"
            extendTexadaRun "$template"
          fi
      fi
    



    else
      echo "generateTexada.sh: Template $formula not recognized"
      exit 1
    fi


    texada_run="$texada_run\\ncat /output/\$MINED_SPECIFICATIONS_FILE"

    #dump to file
    echo -e "$texada_run" > "$output_file"
}


#generate_texada_run "$1" "$2"
#echo "$texada_run"
