cmake_minimum_required(VERSION 3.14)

# set the project name
project("USM-T" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

### COMPILER FLAGS ###
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9")
        message(FATAL_ERROR "clang version must be >= 9")
    endif()
    add_compile_options("-fPIC")
elseif (CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8")
        message(FATAL_ERROR "gcc version must be >= 8")
    endif()
    add_compile_options("-fPIC")
    #Required for youcompleteme
    add_compile_options("-std=c++1z")
    add_compile_options("-Wno-psabi")
    add_link_options("--profile")
endif()


### BUILD TYPE ###
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "${Green}Release mode${Reset}")
    add_compile_options("-O3")
    add_compile_options("-DNDEBUG")
    add_compile_options("-w")

elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    ##add define DEBUG
    add_compile_definitions(DEBUG)
    message(STATUS "${Green}Debug mode${Reset}")
    if (CMAKE_COMPILER_IS_GNUCXX)
        add_compile_options("-ggdb3")
        add_compile_options("-Wall")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options("-ggdb")
        add_compile_options("-Wall")
    else()
        add_compile_options("-g")
        add_compile_options("-Wall")
    endif()

    #   Usually too restrictive
    #    add_compile_options("-Wextra")
    #    add_compile_options("-Wpedantic")

elseif (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    message(STATUS "${Green}Coverage Mode${Reset}")
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "--coverage")
        add_compile_options("--coverage")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options("-fprofile-instr-generate")
        add_compile_options("-fcoverage-mapping")
        set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS}
            "-fprofile-instr-generate -fcoverage-mapping")
    endif()

    SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

    find_program(LCOV_FOUND lcov)
    find_program(GEN_HTML_FOUND genhtml)

    if (NOT LCOV_FOUND)
        message(ERROR "${Red}Using Coverage mode but lcov could not be found${Reset}")
    endif()

    if(NOT GEN_HTML_FOUND)
        message(ERROR "${Red}Using Coverage mode but genhtml could not be found${Reset}")
    endif()

    message(STATUS "${Blue}lcov found${Reset}")
    add_custom_target(coverage
        COMMAND rm -f coverage.info
        COMMAND rm -fr coverage_html
        COMMAND lcov --include *.cc --include *.cpp --include *.hh --include *.i.hh --exclude *googletest* --exclude *antlr4/temporalParser/grammarTemporal/* --exclude *antlr4/temporalParser/grammarSVA/* --exclude *antlr4/propositionParser/grammar/* --exclude *antlr4/varDeclarationParser/grammar/* --exclude *VCD*.cpp --exclude *VCD*.h --exclude *csv-parser/include/internal* --exclude *rapidxml/include/rapidxml.hh  --exclude *third_party* --exclude *location.hh -c -d . -o main_coverage.info
        COMMAND genhtml main_coverage.info --output-directory coverage_html
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/build
        )

endif()

message("-- Building for ${CMAKE_SYSTEM_NAME}")

################CUSTOM TARGETS##################### 
find_program(DOXYGEN_FOUND doxygen)
if (DOXYGEN_FOUND)
    set(current_targets ${current_targets} "docHarm\n")
    message(STATUS "${Blue}doxygen found${Reset}")
    add_custom_target(docHarm COMMAND doxygen ${PROJECT_SOURCE_DIR}/doc/doxygen.cfg)
endif()

find_program(CLANG_FORMAT clang-format)
if (CLANG_FORMAT)
    set(current_targets ${current_targets} "format\n")
    message(STATUS "${Blue}clang_format found${Reset}")
    add_custom_target(format COMMAND find ${PROJECT_SOURCE_DIR}/src/ -iname *.h -o -iname *.hh -o -iname *.cc -o -iname *.cpp | xargs clang-format -style=file -i)
endif()


#Basic configuration
include(FindPackageHandleStandardArgs)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#gtests
add_subdirectory(src/googletest EXCLUDE_FROM_ALL)

#boost
find_package(BOOST1_83)

#spotLTL
find_package(SPOTLTL REQUIRED)
add_compile_definitions(SPOTLTL)

#z3
find_package(Z3 REQUIRED)

#gedlib
find_package(GEDLIB REQUIRED)



### LIBRARIES LINKED TO EVERYONE ######################################

include_directories(
    src/globals/include/
    src/logging/include/
    src/utils/include/
    src/progressbar/include/progresscpp/
    )

#globals
add_subdirectory(src/globals)
link_libraries(globals)

#Logging
add_subdirectory(src/logging)
link_libraries(logging)

#All utils
add_subdirectory(src/utils)
link_libraries(all_utils)

#Progress bar
add_subdirectory(src/progressbar)
link_libraries(progresscpp)

#Exp
add_subdirectory(src/exp)
link_libraries(exp)


### MAIN EXECUTABLE ###############################################

SET(NAME usm-t)
add_executable(${NAME} src/main.cc)

#Command line parser
add_subdirectory(src/commandLineParser)
target_link_libraries(${NAME} commandLineParser)

#c++17 filesystem
target_link_libraries(${NAME} stdc++fs)

######## MINER DEPENDENCIES ########

#antlr4
find_package(ANTLR4 REQUIRED)
add_subdirectory(src/antlr4/varDeclarationParser)
add_subdirectory(src/antlr4/propositionParser)
add_subdirectory(src/antlr4/temporalParser)


#CSV parser
add_subdirectory(src/csv-parser)

#Table printer
add_subdirectory(src/fort)

#Set cover
add_subdirectory(src/paal)

#Sequence alignment
add_subdirectory(src/opal)

########USM-T CORE#########################################
add_subdirectory(src/usm-t)
target_link_libraries(${NAME} usmt_core)

#vcd to csv tool
add_subdirectory(src/miner)
add_subdirectory(src/vcd2csv)

### TESTS & EXAMPLES ##################################
enable_testing()
include (CTest)
add_subdirectory(tests)

######### INSTALLATION #################################
set(CMAKE_INSTALL_MESSAGE NEVER)

INSTALL(TARGETS usm-t 
    RUNTIME DESTINATION usm-t/bin
    LIBRARY DESTINATION usm-t/lib
    ARCHIVE DESTINATION usm-t/lib
    )

#Copy the libraries with their symbolic links
INSTALL(
    CODE
    "execute_process(
        COMMAND
        ${CMAKE_COMMAND}
        -DSpotLib=${SPOTLTL_LIB}
        -DBoostLib=${BoostLinkedLibraries}
        -DBddxLib=${BDDX_LIB}
        -DAntrLib=${ANTLR4_RUNTIME}
        -Dz3Lib=${Z3_LIBRARY}
        -DIP=${CMAKE_INSTALL_PREFIX}
        -P
        ${CMAKE_SOURCE_DIR}/cmake/PostInstall.cmake
    )"
)

