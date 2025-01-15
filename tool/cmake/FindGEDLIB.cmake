include(FindPackageHandleStandardArgs)

set(GEDLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gedlib)

set(GEDLIB_INCLUDE_DIRS ${GEDLIB_ROOT})

list(APPEND GEDLIB_INCLUDE_DIRS
    ${GEDLIB_ROOT}/ext/boost.1.69.0
    ${GEDLIB_ROOT}/ext/eigen.3.3.4/Eigen
    ${GEDLIB_ROOT}/ext/nomad.3.8.1/src
    ${GEDLIB_ROOT}/ext/nomad.3.8.1/ext/sgtelib/src
    ${GEDLIB_ROOT}/ext/lsape.5/cpp/include
    ${GEDLIB_ROOT}/ext/libsvm.3.22
    ${GEDLIB_ROOT}/ext/fann.2.2.0/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usm-t/gedlib/
    )

set(GEDLIB_LIB "")

list(APPEND GEDLIB_LIB
    ${GEDLIB_ROOT}/ext/nomad.3.8.1/lib/libnomad.so
    ${GEDLIB_ROOT}/ext/nomad.3.8.1/lib/libsgtelib.so
    ${GEDLIB_ROOT}/ext/libsvm.3.22/libsvm.so
    ${GEDLIB_ROOT}/ext/fann.2.2.0/lib/libdoublefann.so
    ${GEDLIB_ROOT}/ext/fann.2.2.0/lib/libfann.so
    ${GEDLIB_ROOT}/ext/fann.2.2.0/lib/libfixedfann.so
    ${GEDLIB_ROOT}/ext/fann.2.2.0/lib/libfloatfann.so)

find_package_handle_standard_args(GEDLIB REQUIRED_VARS GEDLIB_INCLUDE_DIRS GEDLIB_LIB)

