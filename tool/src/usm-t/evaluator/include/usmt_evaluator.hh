#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace harm {
class Assertion;
using AssertionPtr = std::shared_ptr<Assertion>;
class Trace;
using TracePtr = std::shared_ptr<Trace>;
} // namespace harm

namespace usmt {
class UseCasePathHandler;
class UseCase;
class EvalReport;
using EvalReportPtr = std::shared_ptr<EvalReport>;
class EditDistanceReport;
using EditDistanceReportPtr = std::shared_ptr<EditDistanceReport>;
class SemanticEquivalenceReport;
using SemanticEquivalenceReportPtr =
    std::shared_ptr<SemanticEquivalenceReport>;
class SyntacticSimilarityReport;
using SyntacticSimilarityReportPtr =
    std::shared_ptr<SyntacticSimilarityReport>;
class Comparator;
struct fault_coverage_t;
struct FlattenedAssertion;

int compareLanguage(const FlattenedAssertion &a1,
                    const FlattenedAssertion &a2);

EvalReportPtr
runSemanticEquivalence(const usmt::UseCase &use_case,
                       const std::string expected_assertion_path);

EvalReportPtr
runEditDistance(const usmt::UseCase &use_case,
                const std::string expected_assertion_path);

EvalReportPtr
runSyntacticSimilarity(const usmt::UseCase &use_case,
                       const std::string expected_assertion_path);

EvalReportPtr runFaultCoverage(const UseCase &use_case,
                               const Comparator comp);

EvalReportPtr evaluate(const usmt::UseCase &use_case,
                       const Comparator &comp);

void evaluateWithEditDistance(
    EditDistanceReportPtr &report,
    const std::unordered_map<
        std::string, std::vector<harm::AssertionPtr>> &assertions);

void evaluateWithSemanticComparison(
    SemanticEquivalenceReportPtr report,
    const std::unordered_map<
        std::string, std::vector<harm::AssertionPtr>> &assertions);

void evaluateWithFaultCoverage(
    const std::vector<harm::AssertionPtr> &selected,
    const harm::TracePtr &originalTrace, fault_coverage_t &fc_result);

struct fault_coverage_t {
  ///maps the assertion to the faults they cover
  std::unordered_map<size_t, std::vector<size_t>> _aidToF;
  ///maps covered faults to the assertion covering them
  std::unordered_map<size_t, std::vector<size_t>> _fToAid;
  //list of faulty traces
  std::vector<std::string> _faultyTraceFiles;
};

void evaluateWithSyntacticSimilarity(
    SyntacticSimilarityReportPtr &report,
    const std::unordered_map<
        std::string, std::vector<harm::AssertionPtr>> &assertions);
} // namespace usmt
