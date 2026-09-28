#include <string>
#include <unordered_map>
#include <vector>

#include "Assertion.hh"
#include "EvalReport.hh"
#include "SimplifiedAssertion.hh"
#include "Trace.hh"
#include "assertion_utils.hh"
#include "globals.hh"
#include "misc.hh"
#include "usmt_evaluator.hh"
#include "gtest/gtest.h"

using namespace harm;
using namespace usmt;
using namespace expression;

static bool equivalent(const std::string &f1, const std::string &f2) {
  clearLanguageEquivalenceCache();
  return areLanguageEquivalent(SimplifiedAssertion(nullptr, f1),
                               SimplifiedAssertion(nullptr, f2));
}

//returns the hybrid similarity between an expected and a mined assertion
static double hybridSimilarity(const std::string &expected,
                               const std::string &mined) {
  std::vector<VarDeclaration> vars;
  for (const std::string &name : {"a", "b", "c"}) {
    vars.emplace_back(name, ExpType::Bool, 1);
  }
  TracePtr trace = generatePtr<Trace>(vars, 1);

  std::unordered_map<std::string, std::vector<AssertionPtr>> assertions;
  assertions["expected"] = {makeAssertion(expected, trace)};
  assertions["mined"] = {makeAssertion(mined, trace)};

  clearLanguageEquivalenceCache();
  HybridReportPtr report = std::make_shared<HybridReport>();
  evaluateWithHybrid(report, assertions);
  return report->_expectedToClosest.at(
                                     assertions["expected"][0]->toString())
      .second;
}

TEST(hybrid_similarityTests, equivalent_is_one) {
  clc::wsilent = 1;
  EXPECT_DOUBLE_EQ(hybridSimilarity("G(a -> b)", "G(!b -> !a)"), 1.f);
  EXPECT_DOUBLE_EQ(hybridSimilarity("G(a -> X(b))", "G(!a || X(b))"),
                   1.f);
  EXPECT_DOUBLE_EQ(
      hybridSimilarity("G({a ##1 b} |-> c)", "G(a -> X(b -> c))"), 1.f);
}

TEST(hybrid_similarityTests, not_equivalent_is_below_one) {
  clc::wsilent = 1;
  EXPECT_LT(hybridSimilarity("G(a -> b)", "G(b -> a)"), 1.f);
  EXPECT_LT(hybridSimilarity("G(a -> X(b))", "G(a -> b)"), 1.f);
}

TEST(hybrid_similarityTests, language_equivalence) {
  EXPECT_TRUE(equivalent("G(a -> b)", "G(!b -> !a)"));
  EXPECT_TRUE(equivalent("G(a -> F(b)) & G(c -> F(b))",
                         "G((a | c) -> F(b))"));
  EXPECT_FALSE(equivalent("G(a -> b)", "G(b -> a)"));
  //"true" is vacuously equivalent to everything, it must never match
  EXPECT_FALSE(equivalent("1", "1"));
  EXPECT_FALSE(equivalent("G(a -> 1)", "1"));
}
