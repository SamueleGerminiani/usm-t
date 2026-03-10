touch /output/.temp
echo "--------------------------------------------"
echo "Running with template [](P0 ->  <> P10)"
echo "--------------------------------------------"
timeout --foreground 600s ./texada -m --parse-mult-prop -f "[](P0 ->  <> P10)" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 
cat /output/.temp

echo "--------------------------------------------"
echo "Running with template []((P0 && P1) ->  <> P10)"
echo "--------------------------------------------"
timeout --foreground 600s ./texada -m --parse-mult-prop -f "[]((P0 && P1) ->  <> P10)" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 
cat /output/.temp

cat /output/$MINED_SPECIFICATIONS_FILE
