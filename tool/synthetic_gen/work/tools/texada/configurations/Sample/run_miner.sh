touch /output/.temp
./texada -m --parse-mult-prop -f "[](P0 ->  P10)" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 

./texada -m --parse-mult-prop -f "[](P0 ->  (P10 && XP11))" --log-file /input/$LOG_FILE --out-file /output/.temp 
cat /output/.temp >> /output/$MINED_SPECIFICATIONS_FILE 

cat /output/$MINED_SPECIFICATIONS_FILE
