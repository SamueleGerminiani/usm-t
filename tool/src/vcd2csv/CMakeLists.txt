SET(NAME vcd2csv)



#############################################
# Sources.
#############################################

SET(SRC src/vcd2csv.cc)
add_executable(${NAME} EXCLUDE_FROM_ALL ${SRC})
set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

#############################################
# Targets.
#############################################
target_link_libraries(${NAME} miner_utils traceReader commandLineParser) 



