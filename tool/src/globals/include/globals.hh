
#pragma once
#include <stddef.h>
#include <string>
#include <vector>

enum class Language;

// command line config
namespace clc {
///--ltlf
extern bool useFiniteSemantics;
///--test
extern std::string testFile;
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

///--continue-on-error
extern bool continueOnError;

///--max-threads
extern size_t maxThreads;
///--sva, --psl, --spotltl
extern Language outputLang;
///--sva-assert
extern bool svaAssert;
} // namespace clc

