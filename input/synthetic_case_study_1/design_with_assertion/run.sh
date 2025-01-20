 #!/bin/sh
rm -rf work
vlib work
vlog -sv *.v
vsim -c -assertdebug -voptargs=+acc work.test_bench -do "run -all; exit"
