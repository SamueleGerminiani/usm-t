#include "EdgeProposition.hh"
#include "Location.hh"
#include "Test.hh"
#include "exp.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include <algorithm>
#include <filesystem>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

//spot includes
#include "AutomataBasedEvaluator.hh"
#include "spot/twa/bddprint.hh"
#include "spot/twa/twa.hh"
#include "spot/twa/twagraph.hh"
#include "spot/twaalgos/translate.hh"
#include <spot/tl/formula.hh>
#include <spot/tl/ltlf.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/remprop.hh>
#include <z3++.h>

#include "Assertion.hh"
#include "CSVtraceReader.hh"
#include "EvalReport.hh"
#include "ProgressBar.hpp"
#include "SimplifiedAssertion.hh"
#include "Trace.hh"
#include "VCDtraceReader.hh"
#include "assertion_utils.hh"
#include "propositionParsingUtils.hh"
#include "temporalParsingUtils.hh"
#include "usmt_evaluator.hh"

//normal include

namespace usmt {
using namespace harm;

//caches shared by the semantic and hybrid evaluators, keyed by the flattened formula
static std::unordered_map<std::string, spot::formula> parsed_formulas;
static std::unordered_map<std::string, bool> vacuity_cache;
static std::map<std::pair<std::string, std::string>, bool>
    language_equivalence_cache;

static const spot::formula &getSpotFormula(const std::string &str) {
  auto it = parsed_formulas.find(str);
  if (it == parsed_formulas.end()) {
    it = parsed_formulas.emplace(str, spot::parse_formula(str)).first;
  }
  return it->second;
}

//"true" is vacuously equivalent to everything
static bool isVacuous(const std::string &str) {
  auto it = vacuity_cache.find(str);
  if (it == vacuity_cache.end()) {
    const spot::formula &p = getSpotFormula(str);
    bool vacuous =
        p.is_tt() || spot::are_equivalent(p, spot::formula::tt());
    it = vacuity_cache.emplace(str, vacuous).first;
  }
  return it->second;
}

bool areLanguageEquivalent(const SimplifiedAssertion &a1,
                           const SimplifiedAssertion &a2) {
  const std::string &s1 = a1.flattened_str;
  const std::string &s2 = a2.flattened_str;

  //if one of them is tt, then we discard it
  if (isVacuous(s1) || isVacuous(s2)) {
    return false;
  }

  auto key = s1 < s2 ? std::make_pair(s1, s2) : std::make_pair(s2, s1);
  auto it = language_equivalence_cache.find(key);
  if (it == language_equivalence_cache.end()) {
    bool equivalent =
        spot::are_equivalent(getSpotFormula(s1), getSpotFormula(s2));
    it = language_equivalence_cache.emplace(key, equivalent).first;
  }
  return it->second;
}

void clearLanguageEquivalenceCache() {
  parsed_formulas.clear();
  vacuity_cache.clear();
  language_equivalence_cache.clear();
}

int compareLanguage(const SimplifiedAssertion &a1,
                    const SimplifiedAssertion &a2) {
  //ret 0 if equivalent, 1 if a1 contains/implies/covers a2, 2 if a2 contains/implies/covers a1, -1 if not
  if (isVacuous(a1.flattened_str) || isVacuous(a2.flattened_str)) {
    return -1;
  }

  if (areLanguageEquivalent(a1, a2)) {
    return 0;
  } else {
    const spot::formula &p1 = getSpotFormula(a1.flattened_str);
    const spot::formula &p2 = getSpotFormula(a2.flattened_str);
    if (spot::contains(p1, p2) == 1) {
      return 1;
    } else if (spot::contains(p2, p1) == 1) {
      return 2;
    }
  }
  return -1;
}

void evaluateWithSemanticComparison(
    SemanticEquivalenceReportPtr report,
    const std::unordered_map<std::string, std::vector<AssertionPtr>>
        &assertions) {

  //Flatten assertions--------------------------------------
  std::unordered_map<std::string, std::string>
      targetToRemap; //not used
  auto simplifiedAssertions =
      getSimplifiedAssertions(assertions.at("expected"),
                              assertions.at("mined"), targetToRemap);

  progresscpp::ParallelProgressBar pb;

  const std::vector<SimplifiedAssertion> &expectedSimpAssertions =
      simplifiedAssertions.at("expected");

  const std::vector<SimplifiedAssertion> &minedSimpAssertions =
      simplifiedAssertions.at("mined");

  pb.addInstance(0, "Semantic Matching...",
                 expectedSimpAssertions.size(), 70);

  for (const auto &sea : expectedSimpAssertions) {
    std::string sea_assertionStr = sea.original->toString();
    if (report->_expectedToCoveredWith.count(sea_assertionStr)) {
      goto increment_pb;
    }
    for (const auto &sma : minedSimpAssertions) {
      //no point in comparing if they have no common variables
      if (getNumberOfCommonVariables(sea.original, sma.original) ==
          0) {
        //Debug
        //std::cout << "Skipping " << sea.original->toString()
        //          << " and " << sma.original->toString()
        //          << " because they have no common variables\n";
        continue;
      }
      int res = compareLanguage(sea, sma);
      std::string sma_assertionStr = sma.original->toString();

      if (res == 0) {
        report->_expectedToCoveredWith[sea_assertionStr] =
            sma_assertionStr;
        //clear the similar ones if they are present
        if (report->_expectedToSimilar.count(sea_assertionStr)) {
          report->_expectedToSimilar.erase(sea_assertionStr);
        }
        goto increment_pb;
      } else if (res != -1) {
        report->_expectedToSimilar[sea.original->toString()].insert(
            sma_assertionStr);
      }
    } //end for mined_assertions

    if (!report->_expectedToSimilar.count(sea_assertionStr)) {
      report->_uncovered.push_back(sea_assertionStr);
    }

  increment_pb:
    pb.increment(0);
    pb.display();
  }
  pb.done(0);
}

EvalReportPtr runSemanticEquivalence(
    const usmt::UseCase &use_case,
    const std::unordered_map<
        std::string, std::vector<harm::AssertionPtr>> &assertions) {

  SemanticEquivalenceReportPtr report =
      std::make_shared<SemanticEquivalenceReport>();

  evaluateWithSemanticComparison(report, assertions);

  //compute final score
  report->_final_score += report->_expectedToCoveredWith.size();
  report->_final_score += report->_expectedToSimilar.size() * 0.5f;

  size_t totExpected = report->_expectedToCoveredWith.size() +
                       report->_expectedToSimilar.size() +
                       report->_uncovered.size();
  report->_final_score /= totExpected;

  report->_noise = ((double)assertions.at("mined").size() -
                    (report->_expectedToCoveredWith.size() +
                     report->_expectedToSimilar.size())) /
                   (double)assertions.at("mined").size();

  report->_noise = (report->_noise < 0.f) ? 0.f : report->_noise;

  return report;
}

} // namespace usmt
