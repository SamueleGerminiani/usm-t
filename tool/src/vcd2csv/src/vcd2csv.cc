#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Trace.hh"
#include "TraceReader.hh"
#include "VCDtraceReader.hh"
#include "commandLineParser.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include <memory>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/// @brief handle all the command line arguments
static void parseCommandLineArguments(int argc, char *args[]);

int main(int arg, char *argv[]) {

  //enforce deterministic rand
  srand(1);

  parseCommandLineArguments(arg, argv);

  if (clc::traceFiles.size() == 2 && clc::traceFiles[1] == "END") {
    //check that clc::dumpPath is a file with .csv extension
    messageErrorIf(
        clc::dumpPath.find(".csv") == std::string::npos,
        "dump-to must be a file with .csv extension when giving as "
        "input a single trace with --vcd, got '" +
            clc::dumpPath + "'");
    std::shared_ptr<harm::TraceReader> tr =
        std::make_shared<harm::VCDtraceReader>(clc::traceFiles[0],
                                               clc::clk);
    auto trace = tr->readTrace();
    dumpTraceAsCSV(trace, clc::dumpPath);
  } else {
    //check that clc::dumpPath is a directory
    messageErrorIf(std::filesystem::exists(clc::dumpPath) &&
                       !std::filesystem::is_directory(clc::dumpPath),
                   "dump-to must be a directory when giving as input "
                   "multiple traces with --vcd-dir, got '" +
                       clc::dumpPath + "'");

    for (const auto &tf : clc::traceFiles) {
      std::shared_ptr<harm::TraceReader> tr =
          std::make_shared<harm::VCDtraceReader>(tf, clc::clk);
      auto trace = tr->readTrace();
      //check if output directory exists otherwise create it

      if (!std::filesystem::exists(clc::dumpPath)) {
        if (!std::filesystem::create_directory(clc::dumpPath)) {
          messageError("Could not create output directory '" +
                       clc::dumpPath + "'");
        }
      }
      //extract the name of the trace file
      std::string fileName = tf;
      fileName = fileName.substr(fileName.find_last_of("/") + 1);
      //remove the extension
      fileName = fileName.substr(0, fileName.find_last_of("."));
      dumpTraceAsCSV(trace, clc::dumpPath + "/" + fileName + ".csv");
    }

    return 0;
  }
}

void parseCommandLineArguments(int argc, char *args[]) {
  //parse the cmd using an external library
  auto result = parseVCD2CSV(argc, args);

  //------------------------
  //vcd cases
  //vcd-ss or vcd-ss + vcd-r == 0 : get signals only in selected scope
  //vcd-ss + vcd-r = n with n>0 : get signals in selected scope and recursive with depth n
  //vcd-ss + vcd-unroll = n with n>0 : get signals in selected scope and recursive with depth n and make a context for each scope
  if (result.count("vcd-ss")) {
    clc::selectedScope = result["vcd-ss"].as<std::string>();
    clc::vcdRecursive = 0;
  }
  if (result.count("vcd-r")) {
    clc::vcdRecursive = safeStoull(result["vcd-r"].as<std::string>());
  }

  messageErrorIf(result.count("vcd-unroll") &&
                     (result.count("vcd-r")),
                 "Can not use 'vcd-unroll' with 'vcd-r'");

  messageErrorIf(
      result.count("vcd-unroll") && !result.count("generate-config"),
      "Can not use 'vcd-unroll' without 'generate-config'");

  if (result.count("vcd-unroll")) {
    clc::vcdUnroll =
        safeStoull(result["vcd-unroll"].as<std::string>());
    messageErrorIf(clc::vcdUnroll == 0,
                   "vcd-unroll must be greater than 0");
  }

  if (result.count("clk")) {
    clc::clk = result["clk"].as<std::string>();
  }

  if (result.count("vcd")) {
    clc::parserType = "vcd";
    clc::traceFiles.push_back(result["vcd"].as<std::string>());
    messageErrorIf(!std::filesystem::exists(clc::traceFiles[0]),
                   "Can not find trace file '" + clc::traceFiles[0] +
                       "'");
    clc::traceFiles.push_back("END");
  }

  if (result.count("vcd-dir")) {
    clc::parserType = "vcd";
    messageErrorIf(!std::filesystem::is_directory(
                       result["vcd-dir"].as<std::string>()),
                   "Can not find directory '" +
                       result["vcd-dir"].as<std::string>() + "'");
    for (const auto &entry : std::filesystem::directory_iterator(
             result["vcd-dir"].as<std::string>())) {
      if (entry.path().extension() == ".vcd") {
        clc::traceFiles.push_back(entry.path().u8string());
      }
    }
    messageErrorIf(clc::traceFiles.empty(),
                   "No vcd trace found in: " +
                       result["vcd-dir"].as<std::string>());
  }

  messageInfo("Using test file: " + clc::testFile);

  if (result.count("dump-to")) {
    clc::dumpPath = result["dump-to"].as<std::string>();
    messageErrorIf(clc::dumpPath.back() == '/' &&
                       !std::filesystem::exists(clc::dumpPath),
                   "Can not find directory path '" + clc::dumpPath +
                       "'");
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
}
