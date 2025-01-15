#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "Evaluator.hh"
#include "ProgressBar.hpp"
#include "TemplateImplication.hh"
#include "Trace.hh"
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

TEST(z3Test, lc1) {

  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
    vars.emplace_back("f_" + std::to_string(i), ExpType::Float, 64);
    vars.emplace_back("i_" + std::to_string(i), ExpType::SInt, 64);
  }

  const TracePtr &trace = generatePtr<Trace>(vars, 1);

  PropositionPtr p1 =
      hparser::parseProposition("b_0 && b_1 || b_2", trace);
  std::cout << p1 << "\n";
  Z3ExpWrapper z3exp_1 = to_z3exp(p1);
  std::cout << *z3exp_1._exp << "\n";

  PropositionPtr p2 = hparser::parseProposition("f_0 >= 5.f", trace);
  std::cout << p2 << "\n";
  Z3ExpWrapper z3exp_2 = to_z3exp(p2);
  std::cout << *z3exp_2._exp << "\n";

  PropositionPtr p3 = hparser::parseProposition("i_0 <= 5.f", trace);
  std::cout << p3 << "\n";
  Z3ExpWrapper z3exp_3 = to_z3exp(p3);
  std::cout << *z3exp_3._exp << "\n";
}

TEST(z3Test, testcase) {

  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < 100; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
    vars.emplace_back("f_" + std::to_string(i), ExpType::Float, 64);
    vars.emplace_back("i_" + std::to_string(i), ExpType::SInt, 64);
  }

  const TracePtr &trace = generatePtr<Trace>(vars, 1);

  size_t times = 100;
  //use chrono to measure time
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  for (size_t i = 0; i < times; i++) {
    PropositionPtr p1 =
        hparser::parseProposition("b_0 && b_1 || b_2", trace);
    z3::Z3TestCaseGenerator gen;
    std::vector<z3::TestCase> tc1 = gen.generateTestCase(p1, 100);
    //for (auto tc : tc1) {
    //  std::cout << tc << "\n";
    //}
  }
  std::chrono::steady_clock::time_point end =
      std::chrono::steady_clock::now();

  std::cout << "Time with compare "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   end - begin)
                   .count()
            << "[ms]" << std::endl;
}
