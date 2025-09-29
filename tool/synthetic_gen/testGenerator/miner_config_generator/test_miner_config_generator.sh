# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

#$1 is a path to the folder containing the generated design and traces

if [ ! -d "$1" ]; then
    echo "Error: Input folder $1 does not exist."
    exit 1
fi

bash "$USMT_ROOT/tool/synthetic_gen/testGenerator/miner_config_generator/miner_config_generator.sh" $1 Sample sample_ clk rst sample_::testbench '{"ant":"a_0,b_0,a_2,b_1,a_1,b_2","con":"c_0,c_1,c_2,d_0,d_2,d_1"}'
