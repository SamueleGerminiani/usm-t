if ls /input/*.csv 1> /dev/null 2>&1; then
  enclosed_files=$(ls /input/*.csv | paste -sd, -)
fi
#append also vcd files if they exist
if ls /input/*.vcd 1> /dev/null 2>&1; then
    vcd_files=$(ls /input/*.vcd | paste -sd, -)
    enclosed_files="$enclosed_files,$vcd_files"
fi

if ls /input/*.v 1> /dev/null 2>&1; then
    v_files=$(ls /input/*.v | paste -sd, -)
    enclosed_as_prompt_files="$enclosed_as_prompt_files,$v_files"
fi

python3 runAgent.py --enclose-files-as-prompt $enclosed_as_prompt_files --enclose-files $enclosed_files --agent-setup /input/$AGENT_INSTRUCTIONS --dump-to /output/query_out.txt --api-key $GEMINI_API_KEY
sed -n '/\[Start\]/,/\[End\]/p' /output/query_out.txt | sed '1d;$d' > /output/$MINED_SPECIFICATIONS_FILE
cat /output/$MINED_SPECIFICATIONS_FILE

