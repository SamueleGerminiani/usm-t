export USMT_ROOT=$(pwd)
export PATH=$USMT_ROOT/tool/third_party/spot/bin/:$PATH
export PATH=$USMT_ROOT/tool/third_party/oss-cad-suite/bin/:$PATH
export PATH=$USMT_ROOT/tool/third_party/hifsuite_linux_x86_stand_alone/:$PATH
#conflicts with clear command in bash, moving it to the generation script
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$USMT_ROOT/tool/third_party/hifsuite_linux_x86_stand_alone/