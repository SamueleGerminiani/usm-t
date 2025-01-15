#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "EvalReport.hh"
#include "FlattenedAssertion.hh"
#include "ProgressBar.hpp"
#include "Test.hh"
#include "Trace.hh"
#include "assertion_utils.hh"
#include "minerUtils.hh"
#include "opal.h"
#include <algorithm>
#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace usmt {
using namespace harm;

std::vector<unsigned char> splitBySpacesCollectRemap(
    const std::string &str,
    std::unordered_map<std::string, unsigned char> &all_tokens) {

  std::vector<unsigned char> ramapped_tokens;
  std::istringstream iss(str);

  std::string token;
  while (iss >> token) {
    if (!all_tokens.count(token)) {
      all_tokens.insert({token, all_tokens.size()});
    }
    ramapped_tokens.push_back(all_tokens.at(token));
  }
  return ramapped_tokens;
}

OpalSearchResult **getSimilarity(int alphabetLength,
                                 int scoreMatrix[],
                                 unsigned char query[],
                                 int queryLength, unsigned char *db[],
                                 int dbLength, int dbSeqsLengths[]) {
  int gapOpenPenalty = 1;
  int gapExtendPenality = 1;

  // Results for each sequence in database
  OpalSearchResult **results = new OpalSearchResult *[dbLength];
  for (int i = 0; i < dbLength; i++) {
    results[i] = new OpalSearchResult();
    opalInitSearchResult(results[i]);
  }

  int resultCode = opalSearchDatabase(
      query, queryLength, db, dbLength, dbSeqsLengths, gapOpenPenalty,
      gapExtendPenality, scoreMatrix, alphabetLength, results,
      OPAL_SEARCH_SCORE, OPAL_MODE_NW, OPAL_OVERFLOW_BUCKETS);

  return results;
}
void evaluateWithSyntacticSimilarity(
    SyntacticSimilarityReportPtr &report,
    const std::unordered_map<std::string, std::vector<AssertionPtr>>
        &assertions) {

  std::unordered_map<std::string, std::string> targetToRemap;

  auto flattenedAssertions =
      getFlattenedAssertions(assertions.at("expected"),
                             assertions.at("mined"), targetToRemap);

  const std::vector<FlattenedAssertion> &expectedFAssertions =
      flattenedAssertions.at("expected");
  const std::vector<FlattenedAssertion> &minedFAssertions =
      flattenedAssertions.at("mined");
  progresscpp::ParallelProgressBar pb;
  pb.addInstance(0, "Computing Syntactic Similarity...",
                 expectedFAssertions.size(), 70);
  for (const auto &fea : expectedFAssertions) {
    std::string ea_assertionStr = fea.original->toString();
    for (const auto &fma : minedFAssertions) {
      std::string ma_assertionStr = fma.original->toString();

      //make the alphabet
      std::unordered_map<std::string, unsigned char> alphabet;
      std::vector<unsigned char> fea_tokens =
          splitBySpacesCollectRemap(fea.flattened_str, alphabet);
      std::vector<unsigned char> fma_tokens =
          splitBySpacesCollectRemap(fma.flattened_str, alphabet);

      //make the alphabet matrix
      int alphabetLength = alphabet.size();
      int scoreMatrix[alphabetLength * alphabetLength];
      //initialize the score matrix all zero expect for the diagonal
      for (int i = 0; i < alphabetLength; i++) {
        for (int j = 0; j < alphabetLength; j++) {
          scoreMatrix[i * alphabetLength + j] = -1;
        }
        scoreMatrix[i * alphabetLength + i] = 1;
      }

      //make db with the mined assertion
      unsigned char *db[1] = {fma_tokens.data()};
      int dbSeqsLengths[1] = {(int)fma_tokens.size()};
      int queryLength = fea_tokens.size();
      int dbLength = 1;

      OpalSearchResult **results = getSimilarity(
          alphabetLength, scoreMatrix, fea_tokens.data(), queryLength,
          db, dbLength, dbSeqsLengths);

      int score = results[0]->score;
      delete results[0];
      delete[] results;
      int max_similarity = fea_tokens.size();
      int min_similarity =
          std::max(fma_tokens.size(), fea_tokens.size());
      int normalized_max = max_similarity + min_similarity;
      double normalized_similarity =
          (score + min_similarity) / (double)normalized_max;

      //debug
      //std::cout << "Score:"<<similarity << "\n";
      //std::cout << "Normalized similarity between " << fea.flattened_str
      //          << " and " << fma.flattened_str << " is "
      //          << (double)nomalized_similarity / (double)normalized_max
      //          << "\n";
      //print the alphabet
      //for (const auto &[token, idx] : alphabet) {
      //  std::cout << token << " " << (int)idx << "\n";
      //}
      //print as token sequence
      //std::cout << "Expected: ";
      //for (auto token : fea_tokens) {
      //  std::cout << (int)token << " ";
      //}
      //std::cout << "\n";
      //std::cout << "Mined   : ";
      //for (auto token : fma_tokens) {
      //  std::cout << (int)token << " ";
      //}
      //std::cout << "\n";

      if (normalized_similarity >=
          report->_expectedToClosest[ea_assertionStr].second) {
        report->_expectedToClosest[ea_assertionStr] =
            std::make_pair(ma_assertionStr, normalized_similarity);
      }

      //std::cout << "-----------------------" << "\n";

      if (normalized_similarity == 1.f) {
        break;
      }
    }
    pb.increment(0);
    pb.display();
  }
  pb.done(0);
}

EvalReportPtr
runSyntacticSimilarity(const usmt::UseCase &use_case,
                       const std::string expected_assertion_path) {

  SyntacticSimilarityReportPtr report =
      std::make_shared<SyntacticSimilarityReport>();

  std::unordered_map<std::string, std::vector<AssertionPtr>>
      assertions = getExpectedMinedAssertions(
          use_case, expected_assertion_path);

  evaluateWithSyntacticSimilarity(report, assertions);

  //compute final score
  for (const auto &[ea, closest] : report->_expectedToClosest) {
    auto &[closest_str, sim] = closest;
    report->_final_score += sim;
  }

  report->_final_score /= report->_expectedToClosest.size();

  report->_noise = (assertions.at("mined").size() -
                    report->_expectedToClosest.size()) /
                   (double)assertions.at("mined").size();

  return report;
}

} // namespace usmt
