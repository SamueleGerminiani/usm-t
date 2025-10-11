# Check if $USMT_ROOT is set
if [ -z "$USMT_ROOT" ]; then
    echo "Error: USMT_ROOT environment variable is not set."
    exit 1
fi

rm -rf $USMT_ROOT/miners/tools/goldminer/runs/*
rm -rf $USMT_ROOT/miners/tools/harm/runs/*
rm -rf $USMT_ROOT/miners/tools/samples2ltl/runs/*
rm -rf $USMT_ROOT/miners/tools/texada/runs/*
rm -rf $USMT_ROOT/miners/tools/external/runs/*
rm -rf $USMT_ROOT/miners/tools/gemini-flash/runs/*

