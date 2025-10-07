install=${1:-0} #default 0, 1 install, 2 uninstall
debug=${2:-0} #default 0, 1 debug mode

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

bash "$USMT_ROOT"/tool/synthetic_gen/testGenerator/wrapper.sh \
"$USMT_ROOT"/tool/synthetic_gen/raw_challenges/Next \
Next \
Next_top \
clk \
"{formula : G(..##1.. |=> ..##2..), nant : 3, ncon : 2, nspec: 10, overlap : 0}" \
rst \
Next_top_bench::Next_top_ \
1000 \
$install \
$debug
