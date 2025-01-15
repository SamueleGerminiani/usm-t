SET(NAME usmt_utils)
project(${NAME})



#############################################
# Sources.
#############################################

SET(TR_SRC src/Test.cc src/UseCasePathHandler.cc)

#############################################
# Targets.
#############################################
add_library(${NAME} ${TR_SRC})
target_include_directories(${NAME} PUBLIC include)





