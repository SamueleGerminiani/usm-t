./texada -m --parse-mult-prop -f "[]((P1) -> X(P3 && P4))" --log-file /input/golden.txt --out-file /output/run1.txt
cat /output/run1.txt
./texada -m --parse-mult-prop -f "[]((X(P2)) -> X(P3 && P4))" --log-file /input/golden.txt --out-file /output/run2.txt
cat /output/run2.txt
./texada -m --parse-mult-prop -f "[]((P1 && X(P2)) -> X(P3))" --log-file /input/golden.txt --out-file /output/run3.txt
cat /output/run3.txt
#merge the results
cat /output/run1.txt /output/run2.txt /output/run3.txt > /output/$MINED_ASSERTIONS_FILE
