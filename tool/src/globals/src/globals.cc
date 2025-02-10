#include "globals.hh"
#include "Language.hh"

#include <limits>
#include <thread>

namespace l3Constants {
size_t MAX_THREADS = std::thread::hardware_concurrency();
} // namespace l3Constants
namespace l2Constants {
size_t MAX_THREADS = std::thread::hardware_concurrency();
} // namespace l2Constants

namespace l1Constants {
size_t MAX_THREADS = std::thread::hardware_concurrency() / 2;
} // namespace l1Constants

namespace clc {

std::vector<std::string> traceFiles;
std::vector<std::string> faultyTraceFiles;
std::string testFile = "";
std::string parserType = "";
std::string clk = "";
std::string reset = "";
std::string evaluatorType = "AutomataBased";

Language outputLang = Language::SpotLTL;
bool svaAssert = false;
size_t maxThreads = std::thread::hardware_concurrency();
bool findMinSubset = false;
std::string dumpPath = "";
bool splitLogic = false;
size_t vcdRecursive = true;
size_t vcdUnroll = 0;
std::string selectedScope = "";
bool silent = false;
bool wsilent = false;
bool isilent = false;
bool psilent = false;
} // namespace clc
