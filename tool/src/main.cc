#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "commandLineParser.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "ustm_core.hh"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace usmt;

/// @brief handle all the command line arguments
static void parseCommandLineArguments(int argc, char *args[]);

int main(int arg, char *argv[]) {

  //enforce deterministic rand
  srand(1);

  //print welcome message
  //std::cout << getIcon() << "\n";

  parseCommandLineArguments(arg, argv);

  run();

  return 0;
}

void parseCommandLineArguments(int argc, char *args[]) {
  //parse the cmd using an external library
  auto result = parseUSMT(argc, args);

  clc::testFile = result["test"].as<std::string>();

  messageErrorIf(!std::filesystem::exists(clc::testFile),
                 "Can not find config file '" + clc::testFile + "'");
  //check that config file is an xml file
  messageErrorIf(clc::testFile.find(".xml") == std::string::npos,
                 "Config file must be an xml file, got '" +
                     clc::testFile + "'");
  messageInfo("Using test file: " + clc::testFile);

  if (result.count("max-threads")) {
    size_t nt = result["max-threads"].as<size_t>();
    messageErrorIf(nt == 0, "max-threads must be > 0 ");
    messageWarningIf(
        nt > std::thread::hardware_concurrency(),
        "This machine should run " +
            std::to_string(std::thread::hardware_concurrency()) +
            " threads at most");

    clc::maxThreads = nt;
    l3Constants::MAX_THREADS = nt;
    l2Constants::MAX_THREADS = nt;
    l1Constants::MAX_THREADS = nt;
  }

  if (result.count("silent")) {
    clc::silent = true;
    // disable cout
    std::cout.rdbuf(NULL);
  }
  if (result.count("isilent")) {
    clc::isilent = true;
  }
  if (result.count("wsilent")) {
    clc::wsilent = true;
  }
  if (result.count("psilent")) {
    clc::psilent = true;
  }

  if (result.count("dump-stat")) {
    clc::dumpStat = true;
  }

  if (result.count("dump-to")) {
    clc::dumpPath = result["dump-to"].as<std::string>();
    messageErrorIf(!std::filesystem::exists(clc::dumpPath),
                   "Can not find dump path '" + clc::dumpPath + "'");
    messageErrorIf(!std::filesystem::is_directory(clc::dumpPath),
                   "Dump path '" + clc::dumpPath +
                       "' is not a directory");
  }
}
