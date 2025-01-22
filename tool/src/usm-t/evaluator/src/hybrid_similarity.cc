#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "EvalReport.hh"
#include "SimplifiedAssertion.hh"
#include "ProgressBar.hpp"
#include "Test.hh"
#include "Trace.hh"
#include "assertion_utils.hh"
#include "gedlibWrapper.hh"
#include "minerUtils.hh"
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

void evaluateWithHybrid(
    HybridReportPtr &report,
    const std::unordered_map<std::string, std::vector<AssertionPtr>>
        &assertions) {

  std::unordered_map<AssertionPtr, SerializedAutomaton>
      expectedToSAutomaton;

  std::unordered_map<AssertionPtr, SerializedAutomaton>
      minedToSAutomaton;

  std::unordered_map<std::string, std::string> targetToRemap;

  auto simplifiedAssertions =
      getSimplifiedAssertions(assertions.at("expected"),
                             assertions.at("mined"), targetToRemap);

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

  for (const auto &[ea, sea] : expectedSimpAssertions) {
    Automaton *aut = nullptr;
    try {
      aut = generateAutomatonFromString(sea, ppack);
    } catch (const std::exception &e) {
      messageWarning("(Edit distance) ignoring specification: " +
                     std::string(e.what()));
      continue;
    }
    expectedToSAutomaton[ea] = serializeAutomaton(aut);
  }

  for (const auto &[ma, sma] : minedSimpAssertions) {
    Automaton *aut = nullptr;
    try {
      aut = generateAutomatonFromString(sma, ppack);
    } catch (const std::exception &e) {
      messageWarning("(Edit distance) ignoring specification: " +
                     std::string(e.what()));
      continue;
    }
    minedToSAutomaton[ma] = serializeAutomaton(aut);
  }

  //Compute the similarity between each expected and mined automaton
  progresscpp::ParallelProgressBar pb;
  pb.addInstance(0, "Computing Edit Distance Similarity...",
                 expectedToSAutomaton.size(), 70);

  for (const auto &[ea, ea_sa] : expectedToSAutomaton) {
    std::string ea_assertionStr = ea->toString();
    for (const auto &[ma, ma_sa] : minedToSAutomaton) {
      std::string ma_assertionStr = ma->toString();

      if (getNumberOfCommonVariables(ea, ma) == 0) {
        //std::cout << "Skipping " << ea->toString() << " and "
        //          << ma->toString()
        //          << " because they have no common variables\n";
        continue;
      }

      double similarity = 0.f;
      //Syntatic equality
      if (ea_assertionStr == ma_assertionStr) {
        similarity = 1.f;
      } else {
        //Edit distance
        similarity = computeHybridSimilarity(ea_sa, ma_sa);
      }

      //std::cout << "Similarity between " << ea->toString() << " and " << ma->toString() << " is " << similarity << "\n";
      if (similarity >=
          report->_expectedToClosest[ea_assertionStr].second) {
        report->_expectedToClosest[ea_assertionStr] =
            std::make_pair(ma_assertionStr, similarity);
      }
      if (similarity == 1.f) {
        break;
      }
    }
    pb.increment(0);
    pb.display();
  }
  pb.done(0);
}

EvalReportPtr
runHybrid(const usmt::UseCase &use_case,
                const std::string expected_assertion_path) {

  HybridReportPtr report =
      std::make_shared<HybridReport>();

  std::unordered_map<std::string, std::vector<AssertionPtr>>
      assertions = getExpectedMinedAssertions(
          use_case, expected_assertion_path);

  evaluateWithHybrid(report, assertions);

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
