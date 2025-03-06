#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/sample ~/usm-t/tool/synthetic_gen/sample_config_gen/benchmark_configs sample sample_top clk "{G(..&&.. |=> F ..&&..),3,2,2,3}" rst sample_top_bench::sample_top_ 1000 $install
