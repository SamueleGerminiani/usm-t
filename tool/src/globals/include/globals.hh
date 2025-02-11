
#pragma once
#include <stddef.h>
#include <string>
#include <vector>

enum class Language;

// command line config
namespace clc {
///--test
extern std::string testFile;
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
///--silent
extern bool silent;
///--wilent
extern bool wsilent;
///--isilent
extern bool isilent;
///--psilent
extern bool psilent;
} // namespace clc

