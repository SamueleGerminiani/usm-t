#!/bin/bash
function generate_harm_xml() {
    local output_file=$1
    local hint_string=$2
    local golden_vcd_name=$3
    local vcd_scope=$4

    #DEBUG
    #echo "hint_string": $hint_string

    # Parse the input string into antecedent and consequence variables
    local ant=$(echo "$hint_string" | grep -oP '{ant\s*:\s*\K[^;}]*')
    local con=$(echo "$hint_string" | grep -oP '\scon\s*:\s*\K[^;}]*')
    local formula=$(echo "$hint_string" | grep -oP 'formula\s*:\s*\K[^;}]*')
    formula="${formula// /}"  # remove spaces
    local nant=$(echo "$hint_string" | grep -oP 'nant\s*:\s*\K[0-9]+')
    local ncon=$(echo "$hint_string" | grep -oP 'ncon\s*:\s*\K[0-9]+')
    local nspec=$(echo "$hint_string" | grep -oP 'nspec\s*:\s*\K[0-9]+')
    local indent1="  "
    local indent2=${indent1}"  "
    local indent3=${indent2}"  "
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
    

    harm_template_xml=""
    imp_sign=$(echo "$formula" | grep -oP '(->|=>|\|->|\|=>)')
    if [[ "$imp_sign" == "=>" || "$imp_sign" == "|=>" ]]; then
      imp_shift="X"
    fi

    harm_effort=0

    #G(..&&.. -> ..&&..)
    if echo "$formula" | grep -Eq '^G\(..&&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..&&..\)$'; then
      #Extract the implication sign
      harm_template_xml="${indent2}<template dtLimits=\"3A,${harm_effort}E\" exp=\"G(..&&.. $imp_sign P0)\" />"
      #only if ncon > 1, add more templates
      if (( ncon > 1 )); then
      harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,${harm_effort}E\" exp=\"G(..&&.. $imp_sign P0 && P1)\" />"
      fi
      if (( ncon > 2 )); then
      harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,${harm_effort}E\" exp=\"G(..&&.. $imp_sign P0 && P1 && P2)\" />"
      fi

    #G(..##N.. -> ..##M..)
    elif echo "$formula" | grep -Eq '^G\(..##([0-9]+)..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..##([0-9]+)..\)$'; then
      N=$(echo "$formula" | sed -n 's/[^>]*##\([0-9]\+\).*/\1/p')
      M=$(echo "$formula" | sed -n 's/.*##\([0-9]\+\).*/\1/p')
      harm_template_xml="${indent2}<template dtLimits=\"3A,3D,${harm_effort}E\" exp=\"G(..##$N.. $imp_sign P0)\" />"
      #only if ncon > 1, add more templates
      if (( ncon > 1 )); then
      harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,3D,${harm_effort}E\" exp=\"G(..##$N.. $imp_sign {P0 ##$M P1})\" />"
      fi
      if (( ncon > 2 )); then
      harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,3D,${harm_effort}E\" exp=\"G(..##$N.. $imp_sign {P0 ##$M P1 ##$M P2})\" />"
      fi

    #G(..#N&.. -> ..#M&..)
    elif echo "$formula" | grep -Eq '^G\(..#([0-9]+)&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..#([0-9]+)&..\)$'; then
      N=$(echo "$formula" | sed -n 's/[^>]*#\([0-9]\+\).*/\1/p')
      M=$(echo "$formula" | sed -n 's/.*#\([0-9]\+\).*/\1/p')
      harm_template_xml="${indent2}<template dtLimits=\"3A,3D,3W,${harm_effort}E\" exp=\"G(..#${N}&.. $imp_sign P0)\" />"
      #only if ncon > 1, add more templates
      if (( ncon > 1 )); then
        harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,3D,3W,${harm_effort}E\" exp=\"G(..#${N}&.. $imp_sign {P0 && P1})\" />"
      fi
      if (( ncon > 2 )); then
        harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,3D,3W,${harm_effort}E\" exp=\"G(..#${N}&.. $imp_sign {(P0 && P1) ##$N P2})\" />"
        harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,3D,3W,${harm_effort}E\" exp=\"G(..#${N}&.. $imp_sign {P0 ##$N (P2 && P3)})\" />"
      harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,3D,${harm_effort}E\" exp=\"G(..#$N&.. $imp_sign {P0 ##$M P1 ##$M P2})\" />"
      fi

    #G(..&&.. -> ..&&.. U ..&&..)
    elif echo "$formula" | grep -Eq '^G\(..&&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*..&&..[[:space:]]*U[[:space:]]*..&&..\)$'; then
      ##Issue an error if ncon < 2
      if (( ncon < 2 )); then
        echo "generateHarm.sh: Until template requires at least 2 consequence variables: $formula"
        exit 1
      fi  

      #harm_template_xml="${indent2}<template dtLimits=\"3A,${harm_effor}E\" exp=\"G(..&&.. $imp_sign P0 W P1)\" />"
      ##only if ncon > 2, add more templates
      #if (( ncon > 2 )); then
      #harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,${harm_effor}E\" exp=\"G(..&&.. $imp_sign P0 W (P1 && P2))\" />"
      #harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,${harm_effor}E\" exp=\"G(..&&.. $imp_sign (P0 && P1) W P2)\" />"
      #

      harm_template_xml="${indent2}<template exp=\"G(P0 -> $imp_shift (P10 W P11))\" />"


      if (( nant > 1 )); then
         harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 -> $imp_shift (P10 W P11))\" />"
      fi

      #only if ncon > 2, add more templates
      if (( ncon > 2 )); then
        harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 -> $imp_shift ((P10 && P11) W P12))\" />"
        harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 -> $imp_shift (P10 W (P11 && P12)))\" />"
        if (( nant > 1 )); then
            harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 -> $imp_shift ((P10 && P11) W P12))\" />"
            harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 -> $imp_shift (P10 W (P11 && P12)))\" />"
        fi
      fi

      if (( ncon > 3 )); then
        harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 -> $imp_shift ((P10 && P11) W (P12 && P13)))\" />"
        if (( nant > 1 )); then
            harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 -> $imp_shift ((P10 && P11) W (P12 && P13)))\" />"
        fi
        if (( nant > 2 )); then
          harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 && P2 -> $imp_shift ((P10 && P11) W (P12 && P13)))\" />"
        fi    

        if (( nant > 3 )); then
          harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 && P2 && P3 -> $imp_shift ((P10 && P11) W (P12 && P13)))\" />"

        fi

      fi



    #G(..&&.. -> F ..&&..)
    elif echo "$formula" | grep -Eq '^G\(..&&..[[:space:]]*(->|=>|\|->|\|=>)[[:space:]]*F[[:space:]]*..&&..\)$'; then
      #harm_template_xml="${indent2}<template dtLimits=\"3A,${harm_effor}E\" exp=\"G(..&&.. $imp_sign F P0)\" />"
      ##only if ncon > 1, add more templates
      #if (( ncon > 1 )); then
      #harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,${harm_effor}E\" exp=\"G(..&&.. $imp_sign F (P0 && P1))\" />"
      #fi
      #if (( ncon > 2 )); then
      #harm_template_xml="$harm_template_xml\n${indent2}<template dtLimits=\"3A,${harm_effor}E\" exp=\"G(..&&.. $imp_sign F (P0 && P1 && P2))\" />"
      #fi
      harm_template_xml="${indent2}<template exp=\"G(P0 -> $imp_shift F P10)\" />"

      if (( nant > 1 )); then
         harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 -> $imp_shift F P10)\" />"
      fi

      #only if ncon > 2, add more templates
      if (( ncon > 1 )); then
        harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 -> $imp_shift F (P10 && P11))\" />"
        if (( nant > 1 )); then
           harm_template_xml="$harm_template_xml\n${indent2}<template exp=\"G(P0 && P1 -> $imp_shift F (P10 && P11))\" />"
        fi
      fi

    else
      echo "generateHarm.sh: Template not recognized: $formula"
      exit 1
    fi

    # Split the antecedent and consequence variables into arrays
    IFS=',' read -ra ant_vars <<< "$ant"
    IFS=',' read -ra con_vars <<< "$con"

    # Open file descriptor to write output to file
    exec 3> "$output_file"

    # Start the XML output
    echo "<harm>" >&3
    printf "${indent1}<context name=\"default\">\n" >&3

    # Generate XML entries for antecedent variables
    for var in "${ant_vars[@]}"; do
        echo "${indent2}<prop exp=\"$var\" loc=\"a, dt\"/>" >&3
    done

    # Generate XML entries for consequence variables
    for var in "${con_vars[@]}"; do
        echo "${indent2}<prop exp=\"$var\" loc=\"c\"/>" >&3
    done

    echo "" >&3

    # Add the remaining static parts of the XML
    printf "$harm_template_xml\n\n" >&3


    echo "${indent2}<sort name=\"causality\" exp=\"1-afct/traceLength\"/>" >&3
    echo "${indent2}<sort name=\"frequency\" exp=\"atct/traceLength\"/>" >&3
    echo "${indent1}</context>" >&3
    echo "</harm>" >&3

    # Close file descriptor
    exec 3>&-
}

function generate_harm_run() {
    local output_file=$1
    local golden_vcd_name=$2
    local vcd_scope=$3
    local clk=$4


    # Open file descriptor to write output to file
    exec 3> "$output_file"

    # Start the XML output
    echo "./harm --clk $clk --conf /input/config.xml --vcd /input/$golden_vcd_name --dump-to /output/\$MINED_SPECIFICATIONS_FILE --vcd-ss $vcd_scope --max-threads \$MAX_THREADS --max-ass 1000 --sample-by-con" >&3

    # Close file descriptor
    exec 3>&-
}


