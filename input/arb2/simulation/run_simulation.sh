vlib work
vlog ../design/arb2_bool.v ../design/arb2_tb.v
vsim -c -voptargs="+acc" -do "vcd file ../traces/bool/t1.vcd; vcd add -r /*; run -all; vcd flush; quit" work.arb2_tb
rm -rf work

