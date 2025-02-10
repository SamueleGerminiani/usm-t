
#pragma once
#include <stddef.h>
#include <string>
#include <vector>

enum class Language;

namespace l3Constants {
/// The maximum number of threads to use for the third layer of parallelization
extern size_t MAX_THREADS;
} // namespace l3Constants
namespace l2Constants {
/// The maximum number of threads to use for the second layer of parallelization
extern size_t MAX_THREADS;
} // namespace l2Constants

namespace l1Constants {
/// The maximum number of threads to use for the first layer of parallelization
extern size_t MAX_THREADS;
} // namespace l1Constants

// command line config
namespace clc {
///--vcd --csv --vcd-dir --csv-dir
extern std::vector<std::string> traceFiles;
///--fd
extern std::vector<std::string> faultyTraceFiles;
///--find-min-subset
extern bool findMinSubset;
///--test
extern std::string testFile;
///csv or vcd
extern std::string parserType;
///--clk
extern std::string clk;
///--reset
extern std::string reset;
extern std::string evaluatorType;
///--sva, --psl, --spotltl
extern Language outputLang;
///--sva-assert
extern bool svaAssert;
///--dump-trace-as-csv
extern std::string dumpTraceAsCSV;
///--max-threads
extern size_t maxThreads;
///--dump-to
extern std::string dumpPath;
///--vcd-r
extern size_t vcdRecursive;
///--vcd-unroll
extern size_t vcdUnroll;
///--vcd-ss
extern std::string selectedScope;
///--silent
extern bool silent;
///--wilent
extern bool wsilent;
///--isilent
extern bool isilent;
///--psilent
extern bool psilent;
} // namespace clc

