
#include "usmt_evaluator.hh"
#include "EvalReport.hh"
#include "Test.hh"
#include "assertion_utils.hh"
#include "message.hh"

namespace usmt {
EvalReportPtr evaluate(const usmt::UseCase &use_case,
                       const Comparator &comp) {

  const UseCasePathHandler &ph = use_case.ph;

  if (comp.with_strategy == "semantic_equivalence") {
    return runSemanticEquivalence(use_case,
                                  ph.ustm_root + "/" + comp.expected);
  } else if (comp.with_strategy == "edit_distance") {
    return runEditDistance(use_case,
                           ph.ustm_root + "/" + comp.expected);
  } else if (comp.with_strategy == "syntactic_similarity") {
    return runSyntacticSimilarity(use_case,
                                  ph.ustm_root + "/" + comp.expected);
  } else if (comp.with_strategy == "fault_coverage") {
    return runFaultCoverage(use_case, comp);

  } else if (comp.with_strategy == "n_mined") {
    auto assertions =
        getExpectedMinedAssertions(use_case, ph.ustm_root + "/" + comp.expected);
    NMinedReportPtr report = std::make_shared<NMinedReport>();
    report->_n_mined_assertions = assertions.at("mined").size();
    report->_n_expected_assertions = assertions.at("expected").size();
    return report;
  }

  messageError("Unsupported strategy '" + comp.with_strategy + "'");
  return {};
}
} // namespace usmt
