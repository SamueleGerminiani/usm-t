#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGenerator/wrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/NextChainOvl ~/usm-t/tool/synthetic_gen/sample_config_gen/NextChainOvl_configs NextChainOvl NextChainOvl_top clk "{G(..##1.. |=> ..##1..),3,3,5,3}" rst NextChainOvl_top_bench::NextChainOvl_top_ 1000 $install
