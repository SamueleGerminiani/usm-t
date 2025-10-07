#include "globals.hh"
#include "Language.hh"

#include <limits>
#include <thread>

namespace clc {
std::string testFile = "";

size_t maxThreads = std::thread::hardware_concurrency();
Language outputLang = Language::SpotLTL;
bool svaAssert = false;
std::string dumpPath = "";
bool useFiniteSemantics = false;

//logging
bool silent = false;
bool wsilent = false;
bool isilent = false;
bool psilent = false;

bool continueOnError = false;

} // namespace clc
