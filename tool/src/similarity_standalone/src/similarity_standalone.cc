#include "opal.h"
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

void performSemanticSimilarity(const SimplifiedAssertion &s1,
                               const SimplifiedAssertion &s2,
                               const PlaceholderPack &ppack) {
  int res = compareLanguage(s1, s2);
  float similarity = -1.f;
  if (res == -1) {
    similarity = 0.f;
  } else if (res == 0) {
    similarity = 1.f;
  } else {
    similarity = 0.5f;
  }

  std::cout << "Semantic similarity is " << similarity << "\n";
}
void performSyntacticSimilarity(const SimplifiedAssertion &s1,
                                const SimplifiedAssertion &s2,
                                const PlaceholderPack &ppack) {

  //make the alphabet
  std::unordered_map<std::string, unsigned char> alphabet;
  std::vector<unsigned char> sea_tokens =
      splitBySpacesCollectRemap(s1.flattened_str, alphabet);
  std::vector<unsigned char> sma_tokens =
      splitBySpacesCollectRemap(s2.flattened_str, alphabet);

  //make the alphabet matrix
  int alphabetLength = alphabet.size();
  int scoreMatrix[alphabetLength * alphabetLength];
  //initialize the score matrix all -1 expect for the diagonal
  for (int i = 0; i < alphabetLength; i++) {
    for (int j = 0; j < alphabetLength; j++) {
      scoreMatrix[i * alphabetLength + j] = -1;
    }
    scoreMatrix[i * alphabetLength + i] = 0;
  }

  //make db with the mined assertion
  unsigned char *db[1] = {sma_tokens.data()};
  int dbSeqsLengths[1] = {(int)sma_tokens.size()};
  int queryLength = sea_tokens.size();
  int dbLength = 1;

  int gapOpenPenalty = 1;
  int gapExtendPenality = 1;

  // Results for each sequence in database
  OpalSearchResult **results = new OpalSearchResult *[dbLength];
  for (int i = 0; i < dbLength; i++) {
    results[i] = new OpalSearchResult();
    opalInitSearchResult(results[i]);
  }

  opalSearchDatabase(sea_tokens.data(), queryLength, db, dbLength,
                     dbSeqsLengths, gapOpenPenalty, gapExtendPenality,
                     scoreMatrix, alphabetLength, results,
                     OPAL_SEARCH_SCORE, OPAL_MODE_NW,
                     OPAL_OVERFLOW_BUCKETS);

  int score = results[0]->score;
  delete results[0];
  delete[] results;

  int max_penality = std::max(sma_tokens.size(), sea_tokens.size());
  double normalized_similarity =
      1.f - ((double)score * -1) / (double)max_penality;

  std::cout << "Syntactic similarity is " << normalized_similarity
            << "\n";
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
  std::cout << s1->toColoredString() << " vs "
            << s2->toColoredString() << "\n";

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

  performSyntacticSimilarity(simp_s1, simp_s2, ppack);
  performSemanticSimilarity(simp_s1, simp_s2, ppack);
  performHybridSimilarity(simp_s1, simp_s2, ppack);

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
