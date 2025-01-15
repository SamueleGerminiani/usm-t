function(addTest test_name src)
    add_executable(${test_name} ${src})
    target_link_libraries (${test_name} gtest_main stdc++fs all_utils progresscpp logging exp commandLineParser temporalParser usmt_core usmt_evaluator)
    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${WORK_DIR})
endfunction()

message("-- Including test cases...")

set(WORK_DIR ${CMAKE_BINARY_DIR})
set(TEST_DIR ${CMAKE_BINARY_DIR}/../tests)

addTest(z3Tests z3Tests.cc)
addTest(gedlibTests gedlibTests.cc)
addTest(holdsTest holdsTest.cc)
addTest(edit_distance_scalabilityTests edit_distance_scalabilityTests.cc)
addTest(semantic_equivalence_scalabilityTests semantic_equivalence_scalabilityTests.cc)
addTest(fault_coverage_scalabilityTests fault_coverage_scalabilityTests.cc)
addTest(syntactic_similarity_scalabilityTests syntactic_similarity_scalabilityTests.cc)
addTest(flattening_scalabilityTests flattening_scalabilityTests.cc)
