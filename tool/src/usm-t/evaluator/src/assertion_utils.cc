
#include "expUtils/expUtils.hh"
#include "usmt_evaluator.hh"

#include "Assertion.hh"
#include "CSVtraceReader.hh"
#include "EvalReport.hh"
#include "Location.hh"
#include "ProgressBar.hpp"
#include "Test.hh"
#include "Trace.hh"
#include "VCDtraceReader.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "temporalParsingUtils.hh"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace harm;
using namespace expression;

namespace usmt {

AssertionPtr makeAssertion(const std::string &assertion_str,
                           const TracePtr &trace) {
  TemporalExpressionPtr te =
      hparser::parseTemporalExpression(assertion_str, trace);
  AssertionPtr new_ass = generatePtr<Assertion>(te);
  //  new_ass->enableEvaluation(trace);
  //  messageErrorIf(!new_ass->holdsOnTrace(),
  //                 "Specification '" + assertion_str +
  //                     "', does not hold on the input traces'");
  return new_ass;
}

std::vector<AssertionPtr>
parseAssertions(const std::vector<std::string> &assStrs,
                TracePtr trace) {

  std::vector<AssertionPtr> assertions;

  progresscpp::ParallelProgressBar pb;
  pb.addInstance(0,
                 "Parsing " + std::to_string(assStrs.size()) +
                     " specifications...",
                 assStrs.size(), 70);
  for (size_t i = 0; i < assStrs.size(); i++) {
    assertions.push_back(makeAssertion(assStrs[i], trace));
    pb.increment(0);
    pb.display();
  }
  pb.done(0);
  return assertions;
}

std::vector<AssertionPtr>
getAssertionsFromFile(const std::string &input_path,
                      const TracePtr &trace) {

  messageErrorIf(!std::filesystem::exists(input_path),
                 "Could not find assertino file'" + input_path + "'");
  messageInfo("Parsing assertion file '" + input_path);
  std::vector<std::string> assStrs;
  std::fstream ass(input_path);

  std::string line = "";
  while (std::getline(ass, line)) {
    assStrs.push_back(line);
  }
  messageErrorIf(assStrs.empty(),
                 "No content found in assertion file '" + input_path +
                     "'");
  return parseAssertions(assStrs, trace);
}

using TraceReaderPtr = std::shared_ptr<TraceReader>;
std::vector<std::string>
recoverTracesInDirectory(const std::string &path,
                         const std::string &extension) {
  messageErrorIf(!std::filesystem::exists(path),
                 "Could not find '" + path + "'");
  messageErrorIf(!std::filesystem::is_directory(path),
                 "Not a directory: '" + path + "'");
  std::vector<std::string> ret;
  for (const auto &entry :
       std::filesystem::directory_iterator(path)) {

    if (entry.path().extension() == extension) {
      ret.push_back(entry.path().u8string());
    }
  }
  return ret;
}

TracePtr parseInputTraces(const usmt::UseCase &use_case) {
  const UseCasePathHandler &ph = use_case.ph;

  std::string trace_prefix = ph.ustm_root + "/input/";

  std::vector<std::string> allTraces;

  for (const auto &input : use_case.input) {
    if (input.type != "vcd" && input.type != "csv") {
      continue;
    }
    std::string trace_path = trace_prefix + input.path;
    messageInfo("Parsing input trace(s) at " + trace_path);
    messageErrorIf(!std::filesystem::exists(trace_path),
                   "path '" + trace_path + "' does not exist");
    if (std::filesystem::is_directory(trace_path)) {
      auto traces_in_dir =
          recoverTracesInDirectory(trace_path, "." + input.type);
      allTraces.insert(allTraces.end(), traces_in_dir.begin(),
                       traces_in_dir.end());

    } else {
      allTraces.push_back(trace_path);
    }

    if (input.type == "vcd") {
      clc::selectedScope = use_case.input[0].scope;
      clc::vcdRecursive = 1;
      TraceReaderPtr tr =
          generatePtr<VCDtraceReader>(allTraces, input.clk);
      return tr->readTrace();
    } else if (input.type == "csv") {
      TraceReaderPtr tr = generatePtr<CSVtraceReader>(allTraces);
      return tr->readTrace();
    }
  }

  messageError("Unsupported trace type");
  return nullptr;
}

TracePtr parseFaultyTrace(const std::string &ftStr) {
  TraceReader *tr;
  if (clc::parserType == "vcd") {
    messageErrorIf(!std::filesystem::exists(ftStr),
                   "Can not find '" + ftStr + "'");
    tr = new VCDtraceReader(ftStr, clc::clk);
    TracePtr t = tr->readTrace();
    delete tr;
    return t;
  } else if (clc::parserType == "csv") {
    messageErrorIf(!std::filesystem::exists(ftStr),
                   "Can not find '" + ftStr + "'");
    tr = new CSVtraceReader(ftStr);
    TracePtr t = tr->readTrace();
    delete tr;
    return t;
  }
  messageError("Uknown parser type");
  return nullptr;
}

static std::map<std::pair<std::string, std::string>, int> cache;

int getNumberOfCommonVariables(const AssertionPtr &a1,
                               const AssertionPtr &a2) {
  std::pair<std::string, std::string> key = {a1->toString(),
                                             a2->toString()};

  if (cache.count(key)) {
    return cache[key];
  }

  std::vector<std::pair<std::string, std::pair<ExpType, size_t>>>
      vars1 = expression::getVars(a1->_formula);
  std::vector<std::pair<std::string, std::pair<ExpType, size_t>>>
      vars2 = expression::getVars(a2->_formula);
  size_t common = 0;
  std::unordered_set<std::string> vars1_set;
  for (const auto &v : vars1) {
    vars1_set.insert(v.first);
  }

  for (const auto &v : vars2) {
    if (vars1_set.count(v.first)) {
      common++;
    }
  }

  cache[key] = common;
  std::pair<std::string, std::string> commutative_key = {
      a2->toString(), a1->toString()};
  cache[commutative_key] = common;

  return common;
}

std::unordered_map<std::string, std::vector<AssertionPtr>>
getExpectedMinedAssertions(
    const usmt::UseCase &use_case,
    const std::string expected_assertion_path) {
  std::unordered_map<std::string, std::vector<AssertionPtr>> ret;

  const std::string MINED_ASSERTIONS_FILE =
      getenv("MINED_ASSERTIONS_FILE");

  const UseCasePathHandler &ph = use_case.ph;
  TracePtr trace = parseInputTraces(use_case);
  auto expected_assertions =
      getAssertionsFromFile(expected_assertion_path, trace);
  messageErrorIf(expected_assertions.empty(),
                 "No expected assertions found in " +
                     expected_assertion_path);
  ret["expected"] = expected_assertions;

  std::vector<AssertionPtr> mined_assertions;
  std::string adapted_output_folder =
      ph.work_path + "adapted/" + MINED_ASSERTIONS_FILE;
  auto mined_assertions_tmp =
      getAssertionsFromFile(adapted_output_folder, trace);
  mined_assertions.insert(mined_assertions.end(),
                          mined_assertions_tmp.begin(),
                          mined_assertions_tmp.end());
  ret["mined"] = mined_assertions;

  return ret;
}

} // namespace usmt
