SET(NAME test_reader)
project(${NAME})



#############################################
# Sources.
#############################################

SET(TR_SRC src/test_reader.cc)

#############################################
# Targets.
#############################################
add_library(${NAME} ${TR_SRC})
target_link_libraries(${NAME} PUBLIC usmt_utils)
target_include_directories(${NAME} PUBLIC include ../../rapidxml/include/)





