./texada -m --parse-mult-prop -f "[]((P1 && X(P2)) -> X[2](P3 && P4))" --log-file /input/$LOG_FILE --out-file /output/$MINED_SPECIFICATIONS_FILE
cat /output/$MINED_SPECIFICATIONS_FILE
