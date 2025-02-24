
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
  messageInfo("Parsing specification file '" + input_path);
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
                 "Recover traces: Could not find '" + path + "'");
  messageErrorIf(!std::filesystem::is_directory(path),
                 "Recover traces: Not a directory: '" + path + "'");
  std::vector<std::string> ret;
  for (const auto &entry :
       std::filesystem::directory_iterator(path)) {

    if (entry.path().extension() == extension) {
      ret.push_back(entry.path().u8string());
    }
  }

  messageErrorIf(
      ret.empty(),
      "Recover traces: could not find any traces in directory '" +
          path + "'");
  return ret;
}

TracePtr parseFaultyCSVTrace(const std::string &ftStr) {
  TraceReader *tr;
  //csv parser
  messageErrorIf(!std::filesystem::exists(ftStr),
                 "Can not find '" + ftStr + "'");
  tr = new CSVtraceReader(ftStr);
  TracePtr t = tr->readTrace();
  delete tr;
  return t;
}

TracePtr parseFaultyVCDTrace(const std::string &ftStr,
                             const VCDTraceReaderConfig vcd_config) {
  TraceReader *tr;
  messageErrorIf(!std::filesystem::exists(ftStr),
                 "Can not find '" + ftStr + "'");
  tr = new VCDtraceReader(ftStr, vcd_config);
  TracePtr t = tr->readTrace();
  delete tr;
  return t;
}

int getNumberOfCommonVariables(const AssertionPtr &a1,
                               const AssertionPtr &a2) {
  static std::map<std::pair<std::string, std::string>, int> cache;
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

  const std::string MINED_SPECIFICATIONS_FILE =
      getenv("MINED_SPECIFICATIONS_FILE");

  const UseCasePathHandler &ph = use_case.ph;
  TracePtr trace = use_case.input.getTrace();
  auto expected_assertions =
      getAssertionsFromFile(expected_assertion_path, trace);
  messageErrorIf(expected_assertions.empty(),
                 "No expected assertions found in " +
                     expected_assertion_path);
  ret["expected"] = expected_assertions;

  std::vector<AssertionPtr> mined_assertions;
  std::string adapted_output_folder =
      ph.work_path + ph.work_adapted + MINED_SPECIFICATIONS_FILE;
  auto mined_assertions_tmp =
      getAssertionsFromFile(adapted_output_folder, trace);
  mined_assertions.insert(mined_assertions.end(),
                          mined_assertions_tmp.begin(),
                          mined_assertions_tmp.end());
  ret["mined"] = mined_assertions;

  return ret;
}

} // namespace usmt
