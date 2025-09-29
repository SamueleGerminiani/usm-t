#launch from syntetic_gen folder
install=${1:-0} #default 0, 1 install, 2 uninstall
debug=${2:-0} #default 0, 1 debug mode

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

bash "$USMT_ROOT"/tool/synthetic_gen/testGenerator/wrapper.sh \
"$USMT_ROOT"/tool/synthetic_gen/raw_challenges/AndChain \
AndChain \
AndChain_top \
clk \
"{G(..&&.. |=> ..&&..),3,3,5,0}" \
rst \
AndChain_top_bench::AndChain_top_ \
1000 \
$install \
$debug
