#!/bin/bash
cd $USMT_ROOT/tool/build
ctest -V -R syntactic_similarity_scalabilityTests
ctest -V -R semantic_equivalence_scalabilityTests
ctest -V -R hybrid_similarity_scalabilityTests
ctest -V -R simplification_scalabilityTests
ctest -V -R fault_coverage_scalabilityTests

