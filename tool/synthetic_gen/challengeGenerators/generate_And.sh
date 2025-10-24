install=${1:-0} #default 0, 1 install, 2 uninstall
debug=${2:-0} #default 0, 1 debug mode

# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

bash "$USMT_ROOT"/tool/synthetic_gen/testGenerator/wrapper.sh \
"$USMT_ROOT"/tool/synthetic_gen/raw_challenges/And \
And \
And_top \
clk \
"{formula : G(..&&.. |-> ..&&..), nant : 2, ncon : 2, nspec: 10, overlap : 0}" \
rst \
And_top_bench::And_top_ \
10000 \
$install \
$debug
