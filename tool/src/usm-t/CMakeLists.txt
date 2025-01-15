SET(NAME usmt_core)



#############################################
# Sources.
#############################################

SET(USMT_SRC interface/src/usmt_core.cc)

#############################################
# Targets.
#############################################
add_library(${NAME} ${USMT_SRC})
target_include_directories(${NAME} PUBLIC interface/include)

add_subdirectory(utils)
add_subdirectory(testReader)
add_subdirectory(adaptor)
add_subdirectory(evaluator)

target_link_libraries(${NAME} test_reader adaptor usmt_evaluator)



