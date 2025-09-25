#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/AndChain ~/usm-t/tool/synthetic_gen/sample_config_gen/AndChain_configs AndChain AndChain_top clk "{G(..&&.. |=> ..&&..),3,3,5,0}" rst AndChain_top_bench::AndChain_top_ 1000 $install
