export USMT_ROOT=$(pwd)
export VARIABLES_MAP_FILE=variables_map.txt
export MINED_SPECIFICATIONS_FILE=mined_specifications.txt
export MAX_N_CORES=$(grep -c ^processor /proc/cpuinfo)
