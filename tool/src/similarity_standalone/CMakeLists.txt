SET(NAME similarity)



#############################################
# Sources.
#############################################

SET(SRC src/similarity_standalone.cc)
add_executable(${NAME} EXCLUDE_FROM_ALL ${SRC})
set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

#############################################
# Targets.
#############################################
target_link_libraries(${NAME} miner_utils usmt_evaluator usmt_utils commandLineParser traceReader) 



