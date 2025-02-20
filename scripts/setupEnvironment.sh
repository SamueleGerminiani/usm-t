#This script sets up the environment variables for the USMT tool.
export USMT_ROOT=$(pwd)
export VARIABLES_MAP_FILE=variables_map.txt
export MINED_SPECIFICATIONS_FILE=mined_specifications.txt
export MAX_N_CORES=$(grep -c ^processor /proc/cpuinfo)

#Synthetic benchmarks generation
export PATH=$USMT_ROOT/tool/third_party/ltlsynt/bin/:$PATH
export PATH=$USMT_ROOT/tool/third_party/oss-cad-suite/bin/:$PATH
export PATH=$USMT_ROOT/tool/third_party/hifsuite_linux_x86_stand_alone/:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$USMT_ROOT/tool/third_party/hifsuite_linux_x86_stand_alone/
