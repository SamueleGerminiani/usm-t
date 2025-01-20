./texada -m --parse-mult-prop -f "[]((P1 && X(P2)) -> X[2](P3 && P4))" --log-file /input/golden.txt --out-file /output/$MINED_ASSERTIONS_FILE
cat /output/$MINED_ASSERTIONS_FILE
