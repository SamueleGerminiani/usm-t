touch /output/.temp
echo "--------------------------------------------"
echo "Running with template [](P0 ->  P10)"
echo "--------------------------------------------"
./texada -m --parse-mult-prop -f "[](P0 ->  P10)" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 
cat /output/.temp

echo "--------------------------------------------"
echo "Running with template [](P0 ->  (P10 && XP11))"
echo "--------------------------------------------"
./texada -m --parse-mult-prop -f "[](P0 ->  (P10 && XP11))" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 
cat /output/.temp

echo "--------------------------------------------"
echo "Running with template []((P0 && XP1) -> X P10)"
echo "--------------------------------------------"
./texada -m --parse-mult-prop -f "[]((P0 && XP1) -> X P10)" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 
cat /output/.temp

echo "--------------------------------------------"
echo "Running with template []((P0 && XP1) -> X (P10 && XP11))"
echo "--------------------------------------------"
./texada -m --parse-mult-prop -f "[]((P0 && XP1) -> X (P10 && XP11))" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 
cat /output/.temp

cat /output/$MINED_SPECIFICATIONS_FILE
