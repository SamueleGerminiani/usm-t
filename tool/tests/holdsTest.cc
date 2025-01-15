#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "CSVtraceReader.hh"
#include "Evaluator.hh"
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

using namespace harm;
using namespace usmt;
using TraceReaderPtr = std::shared_ptr<TraceReader>;

TEST(holdsTest, ht1) {

  auto traces = recoverTracesInDirectory("../tests/holds_test/holds/", ".csv");
  TraceReaderPtr tr = generatePtr<CSVtraceReader>(traces);
  TracePtr trace = tr->readTrace();
  TemporalExpressionPtr te = hparser::parseTemporalExpression(
      "G({a ; b} |-> {##1 c})", trace);

  AssertionPtr new_ass = generatePtr<Assertion>(te);
  new_ass->enableEvaluation(trace);

  ASSERT_EQ(new_ass->holdsOnTrace(), true);
}

TEST(holdsTest, ht2) {

  auto traces = recoverTracesInDirectory("../tests/holds_test/not_holds/", ".csv");
  TraceReaderPtr tr = generatePtr<CSVtraceReader>(traces);
  TracePtr trace = tr->readTrace();
  TemporalExpressionPtr te = hparser::parseTemporalExpression(
      "G({a ; b} |-> {##1 c})", trace);

  AssertionPtr new_ass = generatePtr<Assertion>(te);
  new_ass->enableEvaluation(trace);

  ASSERT_EQ(new_ass->holdsOnTrace(), false);
}
