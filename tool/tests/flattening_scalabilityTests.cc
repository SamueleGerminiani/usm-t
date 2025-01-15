#include <filesystem>
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
#include "FlattenedAssertion.hh"
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
#include <z3++.h>

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
std::vector<AssertionPtr> makeAssertionsFromTemplate(
    std::string _template, size_t max_ant_length,
    size_t max_con_length, size_t number_to_make,
    size_t max_number_of_variables, bool force_uint) {
  std::vector<AssertionPtr> ret;
  TracePtr trace = generateMockTrace(max_number_of_variables);
  std::vector<VarDeclaration> all_vars = trace->getVariables();
  if (force_uint) {
    all_vars = trace->getUnsignedIntVariabes();
  } else {
    all_vars = trace->getBooleanVariabes();
  }

  const std::string assertion_prefix = "G(";
  const std::string assertion_postfix = ")";

  if (_template == "..&&..") {
    for (size_t i = 0; i < number_to_make; i++) {
      //make antecedent
      std::string assertionStr_tmp = assertion_prefix;
      size_t ant_length = rand() % max_ant_length + 1;
      for (size_t j = 0; j < ant_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }
        if (j != ant_length - 1) {
          assertionStr_tmp += " && ";
        }
      }
      assertionStr_tmp += " -> ";
      //make consequent
      size_t on_length = rand() % max_con_length + 1;
      for (size_t j = 0; j < on_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }
        if (j != on_length - 1) {
          assertionStr_tmp += " && ";
        }
      }

      assertionStr_tmp += assertion_postfix;

      //debug
      //std::cout << "Assertion: " << assertionStr_tmp << "\n";
      ret.push_back(makeAssertion(assertionStr_tmp, trace));
    }
  } else if (_template == "..##1..") {
    for (size_t i = 0; i < number_to_make; i++) {
      //make antecedent
      std::string assertionStr_tmp = assertion_prefix + "{";
      size_t ant_length = rand() % max_ant_length + 1;
      for (size_t j = 0; j < ant_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }

        if (j != ant_length - 1) {
          assertionStr_tmp += " ##1 ";
        }
      }
      assertionStr_tmp += "} |-> ";
      //make consequent
      size_t on_length = rand() % max_con_length + 1;
      for (size_t j = 0; j < on_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";
        } else {
          assertionStr_tmp += randomVarName;
        }

        if (j != on_length - 1) {
          assertionStr_tmp += " ##1 ";
        }
      }

      assertionStr_tmp += assertion_postfix;

      //debug
      //std::cout << "Assertion: " << assertionStr_tmp << "\n";
      ret.push_back(makeAssertion(assertionStr_tmp, trace));
    }
  }
  return ret;
}

extern std::map<std::pair<std::string, std::string>, double>
    edge_rel_cost_map;
extern std::map<std::pair<std::string, std::string>, double>
    equivalence_cache;

size_t test_with_parameters(std::string template_,
                            size_t number_of_assertions,
                            size_t number_of_variables,
                            bool force_uint) {
  edge_rel_cost_map.clear();
  equivalence_cache.clear();

  std::vector<AssertionPtr> assertions = makeAssertionsFromTemplate(
      template_, 3, 3, number_of_assertions, number_of_variables,
      force_uint);
  std::unordered_map<std::string, std::vector<AssertionPtr>>
      assertions_map;
  assertions_map["expected"] = assertions;
  assertions_map["mined"] = assertions;

  EditDistanceReportPtr report =
      std::make_shared<EditDistanceReport>();

  std::cout << "Number of comparisons: "
            << assertions_map["expected"].size() *
                   (assertions_map["mined"].size())
            << "\n";
  auto start = std::chrono::high_resolution_clock::now();
  std::unordered_map<std::string, std::string> targetToRemap;
  auto tmp = getFlattenedAssertions(assertions_map.at("expected"),
                                    assertions_map.at("mined"),
                                    targetToRemap);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                               start)
      .count();
}

TEST(flattening_scalabilityTests, flattening_next) {

  //clc::psilent = 1;
  clc::wsilent = 1;

  // clang-format off
  // template, number_of_assertions, number_of_variables, force_uint
  std::tuple<std::string, size_t, size_t, bool> test_cases[] = {
      {"..##1..", 100, 500, false},
      {"..##1..", 200, 500, false},
      {"..##1..", 300, 500, false},
      {"..##1..", 400, 500, false},
      {"..##1..", 500, 500, false},
      {"..##1..", 200, 400, false},
      {"..##1..", 300, 400, false},
      {"..##1..", 400, 400, false},
      {"..##1..", 500, 400, false},
      {"..##1..", 100, 300, false},
      {"..##1..", 200, 300, false},
      {"..##1..", 300, 300, false},
      {"..##1..", 400, 300, false},
      {"..##1..", 500, 300, false},
      {"..##1..", 100, 400, false},
      {"..##1..", 100, 200, false},
      {"..##1..", 200, 200, false},
      {"..##1..", 300, 200, false},
      {"..##1..", 400, 200, false},
      {"..##1..", 500, 200, false},
      {"..##1..", 100, 100, false},
      {"..##1..", 200, 100, false},
      {"..##1..", 300, 100, false},
      {"..##1..", 400, 100, false},
      {"..##1..", 500, 100, false},
      {"..##1..", 100, 100, true},
      {"..##1..", 200, 100, true},
      {"..##1..", 300, 100, true},
      {"..##1..", 400, 100, true},
      {"..##1..", 500, 100, true},
      {"..##1..", 100, 200, true},
      {"..##1..", 200, 200, true},
      {"..##1..", 300, 200, true},
      {"..##1..", 400, 200, true},
      {"..##1..", 500, 200, true},
      {"..##1..", 100, 300, true},
      {"..##1..", 200, 300, true},
      {"..##1..", 300, 300, true},
      {"..##1..", 400, 300, true},
      {"..##1..", 500, 300, true},
      {"..##1..", 100, 400, true},
      {"..##1..", 200, 400, true},
      {"..##1..", 300, 400, true},
      {"..##1..", 400, 400, true},
      {"..##1..", 500, 400, true},
      {"..##1..", 100, 500, true},
      {"..##1..", 200, 500, true},
      {"..##1..", 300, 500, true},
      {"..##1..", 400, 500, true},
      {"..##1..", 500, 500, true},

  };
  // clang-format on

  //dump to csv
  std::string dump_path = "flattening_scalability.csv";

  //delete file if it exists
  if (std::filesystem::exists(dump_path)) {
    std::filesystem::remove(dump_path);
  }
  std::ofstream file(dump_path);
  file << "template,number_of_assertions,number_of_variables,"
          "time,force_uint\n";
  file.close();

  for (auto test_case : test_cases) {
    std::string template_;
    size_t number_of_assertions;
    size_t number_of_variables;
    bool force_uint;
    std::tie(template_, number_of_assertions, number_of_variables,
             force_uint) = test_case;
    std::cout << "Template: " << template_
              << " Number of assertions: " << number_of_assertions
              << " Number of variables: " << number_of_variables
              << "\n";
    size_t time_spent =
        test_with_parameters(template_, number_of_assertions,
                             number_of_variables, force_uint);
    std::ofstream file(dump_path, std::ios_base::app);
    file << template_ << "," << number_of_assertions << ","
         << number_of_variables << "," << time_spent << ","
         << (force_uint ? "int" : "bool") << "\n";
    file.close();
  }
}

