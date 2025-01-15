SET(NAME usmt_evaluator)
project(${NAME})



#############################################
# Sources.
#############################################

SET(EVALUATOR_SRC src/syntactic_similarity.cc src/edit_distance.cc src/semantic_equivalence.cc src/fault_coverage.cc src/assertion_utils.cc src/usmt_evaluator.cc src/z3TestCaseGenerator.cc src/simulation_based.cc src/z3CheckSat.cc src/gedlibWrapper.cc src/FlattenedAssertion.cc)

#############################################
# Targets.
#############################################
add_library(${NAME} ${EVALUATOR_SRC})
target_link_libraries(${NAME} PUBLIC usmt_utils ${SPOTLTL_LIB} miner_utils traceReader set_cover ${GEDLIB_LIB} opal)
target_include_directories(${NAME} PUBLIC include ${SPOTLTLCPP_INCLUDE_DIRS} ${GEDLIB_INCLUDE_DIRS})





