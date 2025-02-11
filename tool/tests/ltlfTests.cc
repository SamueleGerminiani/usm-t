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

TEST(ltlfTest, ltlfTest1) {
  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
  }

  TracePtr trace = generatePtr<Trace>(vars, 5);
  BooleanVariablePtr b_0 = trace->getBooleanVariable("b_0");
  BooleanVariablePtr b_1 = trace->getBooleanVariable("b_1");
  b_0->assign(0, 1);
  b_1->assign(0, 0);
  b_0->assign(1, 0);
  b_1->assign(1, 1);
  b_0->assign(2, 0);
  b_1->assign(2, 0);
  b_0->assign(3, 1);
  b_1->assign(3, 0);
  b_0->assign(4, 0);
  b_1->assign(4, 0);
  trace->setCuts(std::vector<size_t>({trace->getLength() - 1}));

  harm::PlaceholderPack pp;
  pp._tokenToInst["_inst0"] = hparser::parseProposition("b_0", trace);
  pp._tokenToInst["_inst1"] = hparser::parseProposition("b_1", trace);
  std::string assertionStr_tmp = "G(b_0 -> Fb_1)";
  auto assertion = makeAssertion(assertionStr_tmp, trace);

  assertion->enableEvaluation(trace);
  clc::useFiniteSemantics = 0;
  ASSERT_EQ(assertion->holdsOnTrace(), true);

  clc::useFiniteSemantics = 1;
  assertion->enableEvaluation(trace);
  ASSERT_EQ(assertion->holdsOnTrace(), false);
}

