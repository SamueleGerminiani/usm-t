install=${1:-0} #default 0, 1 install, 2 uninstall
debug=${2:-0} #default 0, 1 debug mode

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

bash "$USMT_ROOT"/tool/synthetic_gen/testGenerator/wrapper.sh \
"$USMT_ROOT"/tool/synthetic_gen/raw_challenges/AndUntil \
AndUntil \
AndUntil_top \
clk \
"{formula : G(..&&.. |=> ..&&.. U ..&&..), nant : 2, ncon : 1, nspec : 10, overlap : 0}" \
rst \
AndUntil_top_bench::AndUntil_top_ \
50000 \
$install \
$debug

