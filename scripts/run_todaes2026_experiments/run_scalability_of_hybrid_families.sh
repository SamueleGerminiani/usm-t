#!/bin/bash
cd $USMT_ROOT/tool/build
# each configuration has its own 2h timeout inside the test: disable the ctest one
ctest -V -R hybrid_similarity_families_scalabilityTests --timeout 1000000
