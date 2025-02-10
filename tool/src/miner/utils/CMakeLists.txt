SET(NAME miner_utils)
project(${NAME})


#############################################
# Sources.
#############################################

SET(MINER_UTILS_SRC
    src/Trace.cc
    src/VarDeclaration.cc
    src/Automaton.cc
    src/Evaluator.cc
    src/AutomataBasedEvaluator.cc
    src/Permutator.cc
    src/EdgeProposition.cc
    src/minerUtils.cc
    src/Assertion.cc
    )

add_library(${NAME} ${MINER_UTILS_SRC})
target_include_directories(${NAME} PUBLIC include/ ${SPOTLTLCPP_INCLUDE_DIRS})
target_link_libraries(${NAME} ${SPOTLTL_LIB} fort temporalParser)

