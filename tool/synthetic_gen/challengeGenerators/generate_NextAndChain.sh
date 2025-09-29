#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/NextAndChain ~/usm-t/tool/synthetic_gen/sample_config_gen/NextAndChain_configs NextAndChain NextAndChain_top clk "{G(..#1&.. |=> ..#1&..),3,3,5,0}" rst NextAndChain_top_bench::NextAndChain_top_ 1000 $install
