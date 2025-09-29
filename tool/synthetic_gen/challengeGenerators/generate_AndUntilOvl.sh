#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/AndUntilOvl ~/usm-t/tool/synthetic_gen/sample_config_gen/AndUntilOvl_configs AndUntilOvl AndUntilOvl_top clk "{G(..&&.. |=> ..&&.. U ..&&..),3,6,5,4}" rst AndUntilOvl_top_bench::AndUntilOvl_top_ 1000 $install
