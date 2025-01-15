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

using namespace harm;
using namespace usmt;
using TraceReaderPtr = std::shared_ptr<TraceReader>;

TracePtr generateMockRandomTrace(size_t number_of_variables,
                                 size_t length, bool forceuint) {

  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < number_of_variables; i++) {
    if (forceuint) {
      vars.emplace_back("i_" + std::to_string(i), ExpType::UInt, 32);
    } else {
      vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
    }
  }

  TracePtr trace = generatePtr<Trace>(vars, length);
  size_t which_var_to_fault = rand() % vars.size();
  for (int j = 0; j < vars.size(); ++j) {
    auto &v = vars[j];
    if (forceuint) {
      IntVariablePtr iv = trace->getIntVariable(v.getName());
      for (size_t i = 0; i < length; ++i) {
        iv->assign(i, j == which_var_to_fault ? 0 : 1);
      }
    } else {
      BooleanVariablePtr bv = trace->getBooleanVariable(v.getName());
      for (size_t i = 0; i < length; ++i) {
        bv->assign(i, j == which_var_to_fault ? false : true);
      }
    }
  }
  return trace;
}

TracePtr generateMockTrueTrace(size_t number_of_variables,
                               size_t length, bool forceuint) {

  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < number_of_variables; i++) {
    if (forceuint) {
      vars.emplace_back("i_" + std::to_string(i), ExpType::UInt, 32);
    } else {
      vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
    }
  }

  TracePtr trace = generatePtr<Trace>(vars, length);
  for (int j = 0; j < vars.size(); ++j) {
    auto &v = vars[j];
    if (forceuint) {
      IntVariablePtr iv = trace->getIntVariable(v.getName());
      for (size_t i = 0; i < length; ++i) {
        iv->assign(i, 1);
      }
    } else {
      BooleanVariablePtr bv = trace->getBooleanVariable(v.getName());
      for (size_t i = 0; i < length; ++i) {
        bv->assign(i, true);
      }
    }
  }
  return trace;
}

std::pair<std::vector<AssertionPtr>, TracePtr>
makeAssertionsFromTemplate(std::string _template,
                           size_t max_ant_length,
                           size_t max_con_length,
                           size_t number_to_make,
                           size_t max_number_of_variables,
                           bool force_uint, size_t trace_length) {
  std::vector<AssertionPtr> ret;
  TracePtr trace = generateMockTrueTrace(max_number_of_variables,
                                         trace_length, force_uint);
  std::vector<VarDeclaration> all_vars = trace->getVariables();
  if (force_uint) {
    all_vars = trace->getUnsignedIntVariabes();
  } else {
    all_vars = trace->getBooleanVariabes();
  }

  const std::string assertion_prefix = "G(";
  const std::string assertion_postfix = ")";

  if (_template == "..##1..") {
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
  return {ret, trace};
}
size_t test_with_parameters(std::string template_,
                            size_t number_of_assertions_to_make,
                            size_t number_of_faults,
                            bool force_uint) {

  std::vector<AssertionPtr> assertions;
  size_t trace_length = 1000;
  size_t max_number_of_variables = 100;

  auto assertions_trace = makeAssertionsFromTemplate(
      template_, 3, 3, number_of_assertions_to_make,
      max_number_of_variables, force_uint, trace_length);

  assertions = assertions_trace.first;
  TracePtr golden_trace = assertions_trace.second;
  //make a directory "faulty_traces" in '/tmp' and put the faulty traces there
  std::string faulty_traces = "/tmp/faulty_traces";
  //delete the directory if it exists
  if (std::filesystem::exists(faulty_traces)) {
    std::filesystem::remove_all(faulty_traces);
  }
  std::filesystem::create_directory(faulty_traces);

  fault_coverage_t fc_result;
  for (size_t i = 0; i < number_of_faults; i++) {
    TracePtr trace = generateMockTrueTrace(
        max_number_of_variables, trace_length, force_uint);
    std::string trace_path =
        faulty_traces + "/faulty_" + std::to_string(i) + ".csv";
    dumpTraceAsCSV(trace, trace_path);
    fc_result._faultyTraceFiles.push_back(trace_path);
  }
  clc::parserType = "csv";

  std::chrono ::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  FaultCoverageReportPtr report =
      std::make_shared<FaultCoverageReport>();
  evaluateWithFaultCoverage(assertions, golden_trace, fc_result);
  std::chrono ::steady_clock::time_point end =
      std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                               begin)
      .count();
}

TEST(fault_coverage_scalabilityTests, fault_coverage_next) {
  //clc::wsilent = 1;
  // clang-format off
  std::tuple<std::string,size_t, size_t,bool> test_cases[] = {
    {"..##1..",100,100,true},
    {"..##1..",200,100,true},
    {"..##1..",300,100,true},
    {"..##1..",400,100,true},
    {"..##1..",500,100,true},
    {"..##1..",100,200,true},
    {"..##1..",200,200,true},
    {"..##1..",300,200,true},
    {"..##1..",400,200,true},
    {"..##1..",500,200,true},
    {"..##1..",100,300,true},
    {"..##1..",200,300,true},
    {"..##1..",300,300,true},
    {"..##1..",400,300,true},
    {"..##1..",500,300,true},
    {"..##1..",100,400,true},
    {"..##1..",200,400,true},
    {"..##1..",300,400,true},
    {"..##1..",400,400,true},
    {"..##1..",500,400,true},
    {"..##1..",100,500,true},
    {"..##1..",200,500,true},
    {"..##1..",300,500,true},
    {"..##1..",400,500,true},
    {"..##1..",500,500,true},

    {"..##1..",100,100,false},
    {"..##1..",200,100,false},
    {"..##1..",300,100,false},
    {"..##1..",400,100,false},
    {"..##1..",500,100,false},
    {"..##1..",100,200,false},
    {"..##1..",200,200,false},
    {"..##1..",300,200,false},
    {"..##1..",400,200,false},
    {"..##1..",500,200,false},
    {"..##1..",100,300,false},
    {"..##1..",200,300,false},
    {"..##1..",300,300,false},
    {"..##1..",400,300,false},
    {"..##1..",500,300,false},
    {"..##1..",100,400,false},
    {"..##1..",200,400,false},
    {"..##1..",300,400,false},
    {"..##1..",400,400,false},
    {"..##1..",500,400,false},
    {"..##1..",100,500,false},
    {"..##1..",200,500,false},
    {"..##1..",300,500,false},
    {"..##1..",400,500,false},
    {"..##1..",500,500,false},

  };
  // clang-format on
  //
  //dump to csv
  std::string dump_path = "fault_coverage_scalability.csv";

  //delete file if it exists
  if (std::filesystem::exists(dump_path)) {
    std::filesystem::remove(dump_path);
  }
  std::ofstream file(dump_path);
  file << "template,number_of_assertions_to_make,number_of_faults,force_uint,"
       << "time_spent\n";
  file.close();

  for (auto test_case : test_cases) {
    size_t number_of_assertions_to_make;
    size_t number_of_faults;
    std::string template_;
    bool force_uint;
    std::tie(template_, number_of_assertions_to_make,
             number_of_faults, force_uint) = test_case;

    size_t time_spent =
        test_with_parameters(template_, number_of_assertions_to_make,
                             number_of_faults, force_uint);

    std::cout << "Template: " << template_
              << " Number of assertions to make: "
              << number_of_assertions_to_make
              << " Number of faults: " << number_of_faults
              << (force_uint ? " uint" : " bool")
              << " Time spent: " << time_spent << " ms\n";

    std::ofstream file(dump_path, std::ios_base::app);
    file << template_ << "," << number_of_assertions_to_make << ","
         << number_of_faults << (force_uint ? ",true" : ",false")
         << "," << time_spent << "\n";
    file.close();
  }
}

