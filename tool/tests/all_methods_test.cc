#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "CSVtraceReader.hh"
#include "EvalReport.hh"
#include "Evaluator.hh"
#include "SimplifiedAssertion.hh"
#include "ProgressBar.hpp"
#include "TemplateImplication.hh"
#include "Trace.hh"
#include "TraceReader.hh"
#include "assertion_utils.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "propositionParsingUtils.hh"
#include "temporalParsingUtils.hh"
#include "usmt_evaluator.hh"
#include "visitors/ExpToZ3Visitor.hh"
#include "z3TestCaseGenerator.hh"
#include "gtest/gtest_pred_impl.h"
#include <chrono>
#include <filesystem>

using namespace harm;
using namespace usmt;
using TraceReaderPtr = std::shared_ptr<TraceReader>;
TracePtr generateMockTrace(size_t number_of_variables) {

  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < number_of_variables; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
    vars.emplace_back("i_" + std::to_string(i), ExpType::UInt, 32);
  }

  return generatePtr<Trace>(vars, 1);
}
TEST(all_methodsTetss, amt1) {

  clc::psilent=1;
  TracePtr trace = generateMockTrace(100);
  std::string g_str = "b_0 && Xb_1 -> Xb_2";
  std::string m_str = "{b_0 ##2 b_1} |-> b_2";

  AssertionPtr g = makeAssertion(g_str, trace);
  AssertionPtr m = makeAssertion(m_str, trace);

  std::unordered_map<std::string, std::vector<AssertionPtr>>
      assertions;

  assertions["expected"].push_back(g);
  assertions["mined"].push_back(m);

  SyntacticSimilarityReportPtr s_report =
      std::make_shared<SyntacticSimilarityReport>();

  SemanticEquivalenceReportPtr ss_report =
      std::make_shared<SemanticEquivalenceReport>();
  evaluateWithSemanticComparison(ss_report, assertions);

  evaluateWithSyntacticSimilarity(s_report, assertions);

  HybridReportPtr ed_report =
      std::make_shared<HybridReport>();
  evaluateWithHybrid(ed_report, assertions);

  evaluateWithSyntacticSimilarity(s_report, assertions);

  std::cout << s_report->to_string() << "\n";
  std::cout << ss_report->to_string() << "\n";
  std::cout << ed_report->to_string() << "\n";
}

