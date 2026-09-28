SET(NAME golden_check)



#############################################
# Sources.
#############################################

SET(SRC src/golden_check.cc)
add_executable(${NAME} ${SRC})
set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

#############################################
# Targets.
#############################################
target_link_libraries(${NAME} miner_utils usmt_evaluator usmt_utils commandLineParser traceReader)
