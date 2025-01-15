SET(NAME opal)
project(${NAME})


#############################################
# Sources.
#############################################

SET(OPAL_SRC
    src/opal.cpp
    src/ScoreMatrix.cpp
    )


#############################################
# Targets.
#############################################
#add_library(metric ${METRIC_SRC})
#target_link_libraries(metric propositionParser)

add_library(${NAME} ${OPAL_SRC})
target_include_directories(${NAME} PUBLIC src/)
target_compile_options(${NAME} PRIVATE -march=native)


