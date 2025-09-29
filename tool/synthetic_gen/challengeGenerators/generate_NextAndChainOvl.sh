#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGenerator/wrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/NextAndChainOvl ~/usm-t/tool/synthetic_gen/sample_config_gen/NextAndChainOvl_configs NextAndChainOvl NextAndChainOvl_top clk "{G(..#1&.. |=> ..#1&..),3,3,5,3}" rst NextAndChainOvl_top_bench::NextAndChainOvl_top_ 1000 $install
