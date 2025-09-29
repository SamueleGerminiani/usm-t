#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
bash ~/usm-t/tool/synthetic_gen/testGenerator/wrapper.sh ~/usm-t/tool/synthetic_gen/sample_config_gen/EventuallyOvl ~/usm-t/tool/synthetic_gen/sample_config_gen/EventuallyOvl_configs EventuallyOvl EventuallyOvl_top clk "{G(..&&.. |=> F ..&&..),3,3,5,3}" rst EventuallyOvl_top_bench::EventuallyOvl_top_ 1000 $install
