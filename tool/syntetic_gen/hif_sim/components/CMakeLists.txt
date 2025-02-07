cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_STANDARD 17)

project(synthetic_circuit)

# -----------------------------------------------------------------------------
include_directories(
    ./inc
    ./inc/muffin
)

link_directories(
    ${SCNSL_DIR}/lib
)

set(CMAKE_CXX_FLAGS "-O3")
add_compile_options("-std=c++1z")

#add subdirectories
add_subdirectory(src/cpptracer)
add_executable(
    ${PROJECT_NAME}
    ./src/main.cpp
    ./src/top_module.cpp
    ./src/hif_globals.cpp
    ./src/muffin/muffin_dataTypes.cpp
    ./src/muffin/FaultNode.cpp
)
target_link_libraries(
    ${PROJECT_NAME}
    cpptracer
)

