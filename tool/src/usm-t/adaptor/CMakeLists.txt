SET(NAME adaptor)
project(${NAME})



#############################################
# Sources.
#############################################

SET(ADAPTOR_SRC src/adaptor.cc)

#############################################
# Targets.
#############################################
add_library(${NAME} ${ADAPTOR_SRC})
target_link_libraries(${NAME} PUBLIC usmt_utils)
target_include_directories(${NAME} PUBLIC include)





