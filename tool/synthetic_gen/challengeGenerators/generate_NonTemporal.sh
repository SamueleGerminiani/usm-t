#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/NonTemporal ~/usm-t/tool/synthetic_gen/sample_config_gen/NonTemporal_configs NonTemporal NonTemporal_top clk "{G(..&&.. |=> ..&&..),1,1,6,0}" rst NonTemporal_top_bench::NonTemporal_top_ 1000 $install
