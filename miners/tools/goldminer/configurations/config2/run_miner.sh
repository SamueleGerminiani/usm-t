python2.7 ../src/goldmine.py -m test -u /input/ -S -V -f /input/ --clock "clock:1" -v /input/golden.vcd
find goldmine.out/ -name "*.gold" -exec cat {} + > /output/$MINED_ASSERTIONS_FILE
