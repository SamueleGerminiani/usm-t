#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "SimplifiedAssertion.hh"
#include "Trace.hh"
#include "assertion_utils.hh"
#include "commandLineParser.hh"
#include "exp.hh"
#include "gedlibWrapper.hh"
#include "globals.hh"
#include "message.hh"
#include "minerUtils.hh"
#include "misc.hh"
#include "usmt_evaluator.hh"
#include <memory>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

std::vector<std::string> traceFiles;

using namespace harm;
using namespace usmt;
using namespace expression;

namespace clc {
std::string vars;
std::string spec1;
std::string spec2;
std::string metric;
} // namespace clc

static void parseCommandLineArguments(int argc, char *args[]);

void performHybridSimilarity(const SimplifiedAssertion &s1,
                             const SimplifiedAssertion &s2,
                             const PlaceholderPack &ppack) {

  Automaton *s1_aut = nullptr;
  SerializedAutomaton s1_aut_serialized;
  try {
    s1_aut = generateAutomatonFromString(s1.flattened_str, ppack);
  } catch (const std::exception &e) {
    messageError("(Hybrid similarity) error in specification: " +
                 std::string(e.what()));
  }
  s1_aut_serialized = serializeAutomaton(s1_aut);

  Automaton *s2_aut = nullptr;
  SerializedAutomaton s2_aut_serialized;
  try {
    s2_aut = generateAutomatonFromString(s2.flattened_str, ppack);
  } catch (const std::exception &e) {
    messageError("(Hybrid similarity) error in specification: " +
                 std::string(e.what()));
  }
  s2_aut_serialized = serializeAutomaton(s2_aut);

  auto similarity =
      computeHybridSimilarity(s1_aut_serialized, s2_aut_serialized);

  std::cout << "Hybrid similarity is " << similarity << "\n";
}

int main(int arg, char *argv[]) {

  //enforce deterministic rand
  srand(1);
  parseCommandLineArguments(arg, argv);

  //store the variables in a vector
  std::vector<std::string> variables = parseCSV(clc::vars);
  std::vector<VarDeclaration> vardecs;
  for (const std::string &var : variables) {
    //parse the variable declaration
    vardecs.push_back(VarDeclaration(var, ExpType::Bool, 1));
  }

  TracePtr trace = std::make_shared<Trace>(vardecs, 1);

  AssertionPtr s1 = makeAssertion(clc::spec1, trace);
  AssertionPtr s2 = makeAssertion(clc::spec2, trace);

  std::vector<AssertionPtr> expected_assertions = {s1};
  std::vector<AssertionPtr> mined_assertions = {s2};
  std::unordered_map<std::string, std::string> targetToRemap;
  std::unordered_map<std::string, std::vector<SimplifiedAssertion>>
      simplifiedAssertions = getSimplifiedAssertions(
          expected_assertions, mined_assertions, targetToRemap);

  //Create a mock trace with all the remap targets
  std::vector<VarDeclaration> trace_vars;
  for (auto &[t, r] : targetToRemap) {
    trace_vars.emplace_back(r, ExpType::Bool, 1);
  }
  TracePtr remap_trace = generatePtr<Trace>(trace_vars, 1);
  //make a placeholder pack with the remap targets
  PlaceholderPack ppack;
  for (auto &[t, r] : targetToRemap) {
    ppack._tokenToInst[r] = remap_trace->getBooleanVariable(r);
  }
  const std::vector<SimplifiedAssertion> &expectedSimpAssertions =
      simplifiedAssertions.at("expected");
  const std::vector<SimplifiedAssertion> &minedSimpAssertions =
      simplifiedAssertions.at("mined");

  const SimplifiedAssertion &simp_s1 = expectedSimpAssertions[0];
  const SimplifiedAssertion &simp_s2 = minedSimpAssertions[0];

  if (clc::metric == "hybrid") {
    performHybridSimilarity(simp_s1, simp_s2, ppack);
  }

  return 0;
}

void parseCommandLineArguments(int argc, char *args[]) {
  //parse the cmd using an external library
  auto result = parseSimilarityStandalone(argc, args);

  if (result.count("spec1")) {
    clc::spec1 = result["spec1"].as<std::string>();
  }
  if (result.count("spec2")) {
    clc::spec2 = result["spec2"].as<std::string>();
  }
  if (result.count("metric")) {
    clc::metric = result["metric"].as<std::string>();
  }
  if (result.count("vars")) {
    clc::vars = result["vars"].as<std::string>();
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
