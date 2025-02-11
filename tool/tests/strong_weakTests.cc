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
#include "ProgressBar.hpp"
#include "SimplifiedAssertion.hh"
#include "TemplateImplication.hh"
#include "Trace.hh"
#include "TraceReader.hh"
#include "assertion_utils.hh"
#include "globals.hh"
#include "message.hh"
#include "minerUtils.hh"
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

TEST(strong_weakTests, parse_print1) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "s_always (b_0 s_until b_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "G(b_0 U b_1)");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "s_always (b_0 s_until b_1)");

  assertionStr_tmp = "G(b_0 W b_1)";
  assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "G(b_0 W b_1)");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "always (b_0 until b_1)");
}

TEST(strong_weakTests, parse_print2) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "always(b_0 until b_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "G(b_0 W b_1)");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "always (b_0 until b_1)");
}

TEST(strong_weakTests, parse_print3) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "always(Xb_0)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "GXb_0");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "always nexttime b_0");
}

TEST(strong_weakTests, parse_print4) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "always(X[!]b_0)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "GX[!]b_0");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "always s_nexttime b_0");
}

TEST(strong_weakTests, parse_print5) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "s_always (b_0 R b_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "G(b_0 R b_1)");
}

TEST(strong_weakTests, parse_print6) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "s_always (b_0 M b_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "G(b_0 M b_1)");
}

TEST(strong_weakTests, parse_print7) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "F (b_0 U b_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "F(b_0 U b_1)");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "eventually (b_0 s_until b_1)");
}

TEST(strong_weakTests, parse_print8) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  std::string assertionStr_tmp = "always(X[9!]b_0)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  clc::outputLang = Language::SpotLTL;
  ASSERT_EQ(assertion->toString(), "GX[9!](b_0)");
  clc::outputLang = Language::SVA;
  ASSERT_EQ(assertion->toString(), "always s_nexttime[9] b_0");
}

TEST(strong_weakTests, parse_print9) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");

  clc::outputLang = Language::SpotLTL;
  std::string assertionStr_tmp = "G(b_0 -> b_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);
  ASSERT_EQ(assertion->toString(), "G(b_0 -> b_1)");
}
