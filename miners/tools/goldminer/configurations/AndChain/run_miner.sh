python2.7 ../src/goldmine.py -m AndChain_top -u /input/ -S -V -f /input/ --clock "clk:1" -v /input/golden.vcd ; find goldmine.out/ -name "*.gold" -exec cat {} + > /output/$MINED_SPECIFICATIONS_FILE
