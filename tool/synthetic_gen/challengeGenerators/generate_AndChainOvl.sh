#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/AndChainOvl ~/usm-t/tool/synthetic_gen/sample_config_gen/AndChainOvl_configs AndChainOvl AndChainOvl_top clk "{G(..&&.. |=> ..&&..),3,3,5,3}" rst AndChainOvl_top_bench::AndChainOvl_top_ 1000 $install
