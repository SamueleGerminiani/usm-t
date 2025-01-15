#include <cstddef>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string>
#include <vector>

#include "commandLineParser.hh"

cxxopts::ParseResult parseUSMT(int argc, char *argv[]) {
  try {
    cxxopts::Options options(argv[0], "");
    options.positional_help("[optional args]").show_positional_help();

    std::string file = "";

    // clang-format off
options.add_options()
("test", ".xml containing the usm-t test suite", cxxopts::value<std::string>(), "<FILE>")
( "dump-to", "dump summary report to file", cxxopts::value<std::string>(), "<DIRECTORY>")
( "silent", "disable all outputs")
("wsilent", "disable all warnings")
( "isilent", "disable all infos")
("psilent", "disable all progress bars")
("help", "Show options");
    // clang-format on

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }

    if (result.count("test") == 0) {
      std::cout << "Usage:\n";
      std::cout
          << "usm-t --test <xmlTestFile> [<OptionalArguments...>]"
          << "\n";
      exit(0);
    }

    return result;

  } catch (const cxxopts::OptionException &e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

cxxopts::ParseResult parseVCD2CSV(int argc, char *argv[]) {
  try {
    cxxopts::Options options(argv[0], "");
    options.positional_help("[optional args]").show_positional_help();

    std::string file = "";

    // clang-format off
options.add_options()
("vcd", ".vcd trace", cxxopts::value<std::string>(), "<FILE>")
( "vcd-dir", "path to a directory containing .vcd traces", cxxopts::value<std::string>(), "<DIRECTORY>")
( "vcd-ss", "select a scope of signals in the .vcd trace (disables vcd-r, use " "\"--vcd-r=1\" to enable it again)", cxxopts::value<std::string>(), "<String>")
( "vcd-r", "recursively add signals for sub-scopes, default recursion depth is <max depth>", cxxopts::value<std::string>()->implicit_value( std::to_string((size_t)-1)), "<uint>")
( "vcd-unroll", "create a context for each scope when generating the config file " "(mutually esclusive with vcd-r, default recursion depth is <max depth>)", cxxopts::value<std::string>()->implicit_value( std::to_string((size_t)-1)), "<uint>")
( "clk", "clk signal", cxxopts::value<std::string>(), "<String>")
( "dump-to", "dump csv files do destination. If the the path points to a directory, each traces will be dumped to a sperated file with the same of the original trace but with the .csv extension", cxxopts::value<std::string>(), "<DIRECTORY> or <FILE>")
( "silent", "disable all outputs")
("wsilent", "disable all warnings")
( "isilent", "disable all infos")
("psilent", "disable all progress bars")
( "name", "name of this execution (used when dumping statistics)", cxxopts::value<std::string>(), "<String>")
("help", "Show options");
    // clang-format on

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }

    if (((result.count("vcd") == 1 || result.count("vcd-dir") == 1) &&
         result.count("clk") == 0) ||
        (result.count("vcd") == 0 && result.count("vcd-dir") == 0) || result.count("dump-to") == 0) {
      std::cout << "Usage:\n";
      std::cout << "vcd2csv [--vcd <vcdFile> | --vcd-dir "
                   "<dirPath>] --clk <clk> [<OptionalArguments...>]\n";
      exit(0);
    }

    return result;

  } catch (const cxxopts::OptionException &e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

