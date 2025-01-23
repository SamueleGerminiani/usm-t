python2.7 ../src/goldmine.py -m $TOP_MODULE -u /input/ -S -V -f /input/ --clock $CLK:1 -v /input/$VCD_FILE
find goldmine.out/ -name "*.gold" -exec cat {} + > /output/$MINED_ASSERTIONS_FILE
