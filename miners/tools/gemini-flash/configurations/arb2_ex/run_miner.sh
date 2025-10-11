input_files=$(ls /input/*.v | paste -sd, -)

python3 runAgent.py --input "$input_files" --agent-setup /input/$AGENT_INSTRUCTIONS --dump-to /output/query_out.txt --api-key $GEMINI_API_KEY

sed -n '/\[Start\]/,/\[End\]/p' /output/query_out.txt | sed '1d;$d' > /output/$MINED_SPECIFICATIONS_FILE

cat /output/$MINED_SPECIFICATIONS_FILE
