#include "EvalReport.hh"
#include "Test.hh"
#include "adaptor.hh"
#include "fort.h"
#include "fort.hpp"
#include "globals.hh"
#include "misc.hh"
#include "test_reader.hh"
#include "usmt_evaluator.hh"
#include "ustm_core.hh"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <sstream>
#include <vector>

namespace usmt {

class BestUseCase {
public:
  BestUseCase() = default;

  void addUseCase(const std::string &use_case_id, double value,
                  bool higher_is_better = true) {
    bool is_better = (_best_value == -1.f) ||
                     (higher_is_better && value > _best_value) ||
                     (!higher_is_better && value < _best_value);
    if (is_better) {
      _best_value = value;
      _same_value_use_cases.clear();
      _same_value_use_cases.push_back(use_case_id);
    } else if (value == _best_value) {
      _same_value_use_cases.push_back(use_case_id);
    }
  }

  double _best_value = -1.f;
  std::vector<std::string> _same_value_use_cases;
};

void appendCSVLineToFile(const std::string &filename,
                         const std::vector<std::string> &line) {
  std::ofstream file(filename, std::ios_base::app);
  for (size_t i = 0; i < line.size(); i++) {
    file << line[i];
    if (i != line.size() - 1) {
      file << ",";
    }
  }
  file << "\n";
  file.close();
}

void run() {

  std::vector<Test> tests = readTestFile(clc::testFile);

  for (auto &test : tests) {
    messageInfo("Running test " + test.name);
    std::string summaryReportDumpPath =
        clc::dumpPath + "/summary_report_" + test.name + ".csv";
    if (clc::dumpPath != "" &&
        std::filesystem::exists(summaryReportDumpPath)) {
      std::filesystem::remove(summaryReportDumpPath);
    }

    //print the header of the table
    fort::utf8_table table;
    table.set_border_style(FT_NICE_STYLE);

    std::vector<std::string> line;
    table << fort::header;
    line.push_back("Use Case");
    for (const auto &comparator : test.comparators) {
      line.push_back(comparator.with_strategy);
    }
    table.range_write_ln(std::begin(line), std::end(line));
    appendCSVLineToFile(summaryReportDumpPath, line);

    std::unordered_map<std::string, std::vector<EvalReportPtr>>
        useCaseToEvalReports;

    for (auto &use_case : test.use_cases) {
      initPathHandler(use_case);

      const UseCasePathHandler &ph = use_case.ph;

      //ADAPT THE INPUT--------------------------------
      adaptInput(use_case);

      //PREPARE THE EXECUTION-------------------------------
      //count number of run
      messageErrorIf(
          std::count_if(
              use_case.configs.begin(), use_case.configs.end(),
              [](const Config &c) { return c.type == "run"; }) != 1,
          "Multiple run configurations found in use case " +
              use_case.usecase_id);
      //copy the conf input files to the input folder
      for (const auto &conf : use_case.configs) {
        if (conf.type == "input") {
          std::filesystem::copy(ph.configurations_path + conf.path,
                                ph.work_path + "input/");
        } else if (conf.type == "run") {
          std::filesystem::copy(ph.configurations_path + conf.path,
                                ph.work_path + ph.work_input +
                                    "run_miner.sh");
        } else {
          messageError("Configuration type '" + conf.type +
                       "' not supported in '" + use_case.usecase_id +
                       "'");
        }
      }

      //RUN THE MINER---------------------------------------
      std::string run_container_command = "";
      run_container_command = "bash " + ph.run_container_path;
      //add the shared input and output paths
      run_container_command += " " + ph.work_path + ph.work_input;
      run_container_command += " " + ph.work_path + ph.work_output;
      //add the command to be executed in the container
      run_container_command += " \"bash /input/run_miner.sh\"";
      messageInfo("Running '" + use_case.miner_name + "'");

      //start the timer
      TemporalReportPtr tr = std::make_shared<TemporalReport>();
      auto start = std::chrono::high_resolution_clock::now();
      systemCheckExit(run_container_command);
      auto stop = std::chrono::high_resolution_clock::now();
      tr->_timeMS =
          std::chrono::duration_cast<std::chrono::milliseconds>(stop -
                                                                start)
              .count();

      //ADAPT THE OUTPUT--------------------------------
      adaptOutput(use_case);

      //EVAL-----------------------------------------------
      for (const auto &comp : test.comparators) {
        //skip time_to_mine comparator which is implicit
        if (comp.with_strategy == "time_to_mine") {
          useCaseToEvalReports[use_case.usecase_id].push_back(tr);
          continue;
        }
        EvalReportPtr er = evaluate(use_case, comp);
        std::cout << er->to_string() << "\n";
        er->dumpTo(ph.work_path + ph.work_eval);
        useCaseToEvalReports[use_case.usecase_id].push_back(er);
      }
    } //end of use cases

    //report to best use case
    std::unordered_map<std::string, BestUseCase>
        strategyToBestUseCase;

    //AGGREGATE THE EVALUATION REPORTS-------------------
    for (const auto &[usecase_id, report] : useCaseToEvalReports) {
      std::vector<std::string> line;
      line.push_back(usecase_id);
      for (const auto &er : report) {
        //smart pointer cast
        if (std::dynamic_pointer_cast<FaultCoverageReport>(er)) {
          FaultCoverageReportPtr fcr =
              std::dynamic_pointer_cast<FaultCoverageReport>(er);
          line.push_back(
              to_string_with_precision(fcr->fault_coverage, 2) +
              " (min cov: " +
              std::to_string(fcr->_minCoveringAssertions.size()) +
              ")");
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, fcr->fault_coverage);
        } else if (std::dynamic_pointer_cast<
                       SemanticEquivalenceReport>(er)) {
          SemanticEquivalenceReportPtr evmr =
              std::dynamic_pointer_cast<SemanticEquivalenceReport>(
                  er);
          line.push_back(
              to_string_with_precision(evmr->_final_score, 2) +
              " (noise: " +
              to_string_with_precision(evmr->_noise, 2) + ")");
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, evmr->_final_score);
        } else if (std::dynamic_pointer_cast<EditDistanceReport>(
                       er)) {
          EditDistanceReportPtr evmr =
              std::dynamic_pointer_cast<EditDistanceReport>(er);
          line.push_back(
              to_string_with_precision(evmr->_final_score, 2) +
              " (noise: " +
              to_string_with_precision(evmr->_noise, 2) + ")");
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, evmr->_final_score);
        } else if (std::dynamic_pointer_cast<
                       SyntacticSimilarityReport>(er)) {
          SyntacticSimilarityReportPtr evmr =
              std::dynamic_pointer_cast<SyntacticSimilarityReport>(
                  er);
          line.push_back(
              to_string_with_precision(evmr->_final_score, 2) +
              " (noise: " +
              to_string_with_precision(evmr->_noise, 2) + ")");
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, evmr->_final_score);
        } else if (std::dynamic_pointer_cast<TemporalReport>(er)) {
          TemporalReportPtr tr =
              std::dynamic_pointer_cast<TemporalReport>(er);
          line.push_back(to_string_with_precision(
                             (double)tr->_timeMS / 1000.f, 2) +
                         "s");

          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, (double)tr->_timeMS / 1000.f, false);
        } else {
          messageError("Unknown report type");
        }

      } //end of reports
      table.range_write_ln(std::begin(line), std::end(line));
      appendCSVLineToFile(summaryReportDumpPath, line);
    } // end of useCaseToEvalReports

    std::cout << table.to_string() << std::endl;

    //print the best use case
    for (const auto &[strategy, best] : strategyToBestUseCase) {
      std::string best_use_cases = "";
      for (const auto &use_case : best._same_value_use_cases) {
        best_use_cases += use_case + " ";
      }
      messageInfo("Best " + strategy + ": " + best_use_cases);
    }

  } //end of tests
}

} // namespace usmt
