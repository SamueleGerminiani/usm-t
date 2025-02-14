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
#include "xmlUtils.hh"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <map>
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

// Function to convert numerical value to a colored string based on min and max values
std::string colorize(double value, double min, double max,
                     bool reverse) {
  std::vector<std::string> colors = {
      "\033[31m", // Red
      "\033[33m", // Yellow
      "\033[32m", // Green
  };

  if (min == max) {
    std::ostringstream oss;
    if (reverse) {
      oss << colors.front() << to_string_with_precision(value, 2)
          << "\033[0m";
    } else {
      oss << colors.back() << to_string_with_precision(value, 2)
          << "\033[0m";
    }
    return oss.str();
  }

  int color_index = static_cast<int>((value - min) / (max - min) *
                                     (int)colors.size());
  color_index =
      std::min(std::max(color_index, 0), (int)colors.size() - 1);

  if (reverse) {
    color_index = colors.size() - 1 - color_index;
  }

  std::ostringstream oss;
  oss << colors[color_index] << to_string_with_precision(value, 2)
      << "\033[0m";
  return oss.str();
}

std::vector<std::vector<std::string>>
processTable(const std::vector<std::vector<std::string>> &table_rows,
             const std::vector<size_t> &heatmap_configuration_col,
             size_t skip_top_rows = 0) {

  std::vector<std::vector<std::string>> result = table_rows;
  size_t num_columns = table_rows[0].size();

  for (size_t col = 0; col < num_columns; ++col) {
    if (heatmap_configuration_col[col] == 0) {
      continue;
    }
    double min_val = std::numeric_limits<double>::max();
    double max_val = std::numeric_limits<double>::min();
    std::vector<double> column_values;

    // Extract values and find min/max
    for (size_t i = skip_top_rows; i < table_rows.size(); i++) {
      const auto &row = table_rows[i];
      double val = safeStod(row[col]);
      min_val = std::min(min_val, val);
      max_val = std::max(max_val, val);
      column_values.push_back(val);
    }

    // Colorize values
    for (size_t row = skip_top_rows; row < table_rows.size(); ++row) {
      result[row][col] =
          colorize(column_values[row - skip_top_rows], min_val,
                   max_val, heatmap_configuration_col[col] == 2);
    }
  }

  return result;
}

//export to docker -e
std::string
serializeForDocker(std::set<ExportedVariable> &exportedVariables) {
  std::string ret = "\"";
  for (const auto &ev : exportedVariables) {
    ret += "-e " + ev.name + "=" + ev.value + " ";
  }
  ret.pop_back();
  ret += "\"";

  return ret;
}
void dumpConfigStandalone(const UseCase &uc, const Test &test,
                          std::string dump_file) {
  std::ofstream file(dump_file);

  file << "<usm-t>\n";
  file << rapidxml::xmlNodeToString(uc.input.xml_input);
  file << rapidxml::xmlNodeToString(uc.xml_usecase);
  file << "  <test";
  std::map<std::string, std::string> allAttributes =
      getAllAttributes(test.xml_test);
  if (!allAttributes.empty()) {
    for (auto attr : allAttributes) {
      file << " " << attr.first << "=\"" << attr.second << "\"";
    }
  }
  file << ">\n";
  rapidxml::XmlNodeList usecaseNodes;
  getNodesFromName(test.xml_test, "usecase", usecaseNodes);
  auto found_us = std::find_if(
      usecaseNodes.begin(), usecaseNodes.end(),
      [&uc](rapidxml::xml_node<> *n) {
        return getAttributeValue(n, "id", "") == uc.usecase_id;
      });
  if (found_us != usecaseNodes.end()) {
    file << "  " << rapidxml::xmlNodeToString(*found_us);
  }

  rapidxml::XmlNodeList externalNodes;
  getNodesFromName(test.xml_test, "external", externalNodes);
  found_us = std::find_if(externalNodes.begin(), externalNodes.end(),
                          [&uc](rapidxml::xml_node<> *n) {
                            return getAttributeValue(n, "id", "") ==
                                   uc.usecase_id;
                          });
  if (found_us != externalNodes.end()) {
    file << "  " << rapidxml::xmlNodeToString(*found_us);
  }

  rapidxml::XmlNodeList compareNodes;
  getNodesFromName(test.xml_test, "compare", compareNodes);
  for (auto compareNode : compareNodes) {
    file << "  " << rapidxml::xmlNodeToString(compareNode);
  }

  rapidxml::XmlNodeList expectedNodes;
  getNodesFromName(test.xml_test, "expected", expectedNodes);
  for (auto expectedNode : expectedNodes) {
    file << "  " << rapidxml::xmlNodeToString(expectedNode);
  }

  file << "  </test>\n";
  file << "</usm-t>\n";
  file.close();

  messageErrorIf(
      std::filesystem::exists(dump_file) == false,
      "Could not dump the standalone configuration file '" +
          dump_file + "'");
}

void run() {

  std::vector<Test> tests = readTestFile(clc::testFile);

  for (auto &test : tests) {
    std::vector<std::vector<std::string>> table_rows;
    std::vector<size_t> heatmap_configuration_col = {0};

    messageInfo("Running test " + test.name);
    std::string summaryReportDumpPath =
        clc::dumpPath + "/summary_report_" + test.name + ".csv";
    if (clc::dumpPath != "" &&
        std::filesystem::exists(summaryReportDumpPath)) {
      std::filesystem::remove(summaryReportDumpPath);
    }

    //print the header of the table
    fort::utf8_table table;
    table.set_border_style(FT_PLAIN_STYLE);

    //header
    std::vector<std::string> line_header;
    table << fort::header;
    line_header.push_back("Use Case");
    for (const auto &comparator : test.comparators) {
      line_header.push_back(comparator.with_strategy);
      if (comparator.with_strategy != "time_to_mine" &&
          comparator.with_strategy != "n_mined") {
        line_header.push_back("");
      }
    }

    table_rows.push_back(line_header);
    appendCSVLineToFile(summaryReportDumpPath, line_header);

    std::vector<std::string> line_subheader;
    table << fort::header;
    line_subheader.push_back("");
    for (const auto &comparator : test.comparators) {
      if (comparator.with_strategy == "time_to_mine") {
        line_subheader.push_back("");
        heatmap_configuration_col.push_back(2);
      } else if (comparator.with_strategy == "fault_coverage") {
        line_subheader.push_back("FC");
        heatmap_configuration_col.push_back(1);
        line_subheader.push_back("MC");
        heatmap_configuration_col.push_back(2);
      } else if (comparator.with_strategy == "n_mined") {
        line_subheader.push_back("");
        heatmap_configuration_col.push_back(0);
      } else {
        line_subheader.push_back("MS");
        heatmap_configuration_col.push_back(1);
        line_subheader.push_back("RS");
        heatmap_configuration_col.push_back(2);
      }
    }
    table_rows.push_back(line_subheader);
    appendCSVLineToFile(summaryReportDumpPath, line_subheader);

    std::map<std::string, std::vector<EvalReportPtr>>
        useCaseToEvalReports;

    for (auto &use_case : test.use_cases) {
      initPathHandler(use_case);

      const UseCasePathHandler &ph = use_case.ph;
      bool external = use_case.external_spec_file_path != "";
      TemporalReportPtr tr = std::make_shared<TemporalReport>();

      if (!external) {
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
          if (conf.type == "support") {
            std::filesystem::copy(ph.configurations_path + conf.path,
                                  ph.work_path + ph.work_input);
          } else if (conf.type == "run") {
            std::filesystem::copy(ph.configurations_path + conf.path,
                                  ph.work_path + ph.work_input +
                                      "run_miner.sh");
          } else {
            messageError("Configuration type '" + conf.type +
                         "' not supported in '" +
                         use_case.usecase_id + "'");
          }
        }

        //RUN THE MINER---------------------------------------
        std::string run_container_command = "";
        run_container_command = "bash " + ph.run_container_path;
        run_container_command += " " + use_case.docker_image;
        //add the shared input and output paths
        run_container_command += " " + ph.work_path + ph.work_input;
        run_container_command += " " + ph.work_path + ph.work_output;
        //add the command to be executed in the container
        run_container_command +=
            " \"bash /" + ph.work_input + "run_miner.sh\"";
        //add the exported variables
        if (!use_case.exports.empty()) {
          std::string exported_list =
              serializeForDocker(use_case.exports);
          run_container_command += " " + exported_list;
        }
        messageInfo("Running '" + use_case.miner_name + "'");

        //start the timer
        auto start = std::chrono::high_resolution_clock::now();
        systemCheckExit(run_container_command);
        auto stop = std::chrono::high_resolution_clock::now();
        tr->_timeMS =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                stop - start)
                .count();
      } else {
        //external use case
        messageInfo("External use case " + use_case.usecase_id);
        messageErrorIf(getenv("MINED_SPECIFICATIONS_FILE") == nullptr,
                       "MINED_SPECIFICATIONS_FILE is not set");
        std::string mined_file_name =
            getenv("MINED_SPECIFICATIONS_FILE");
        //copy the external spec file to the output folder
        std::filesystem::copy(ph.external_spec_file_path,
                              ph.work_path + ph.work_output +
                                  mined_file_name);
      }
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

      //DUMP THE CONFIG STANDALONE-------------------------
      dumpConfigStandalone(use_case, test,
                           ph.work_path +
                               ph.work_test_config_standalone +
                               "config.xml");
      messageInfo("Dumping standalone test to '" + ph.work_path +
                  ph.work_test_config_standalone + "config.xml'");

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
              to_string_with_precision(fcr->fault_coverage, 2));
          line.push_back(
              std::to_string(fcr->_minCoveringAssertions.size()));
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, fcr->fault_coverage);
        } else if (std::dynamic_pointer_cast<
                       SemanticEquivalenceReport>(er)) {
          SemanticEquivalenceReportPtr evmr =
              std::dynamic_pointer_cast<SemanticEquivalenceReport>(
                  er);
          line.push_back(
              to_string_with_precision(evmr->_final_score, 2));
          line.push_back(to_string_with_precision(evmr->_noise, 2));
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, evmr->_final_score);
        } else if (std::dynamic_pointer_cast<HybridReport>(er)) {
          HybridReportPtr evmr =
              std::dynamic_pointer_cast<HybridReport>(er);
          line.push_back(
              to_string_with_precision(evmr->_final_score, 2));
          line.push_back(to_string_with_precision(evmr->_noise, 2));
          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, evmr->_final_score);
        } else if (std::dynamic_pointer_cast<
                       SyntacticSimilarityReport>(er)) {
          SyntacticSimilarityReportPtr evmr =
              std::dynamic_pointer_cast<SyntacticSimilarityReport>(
                  er);
          line.push_back(
              to_string_with_precision(evmr->_final_score, 2));
          line.push_back(to_string_with_precision(evmr->_noise, 2));
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
        } else if (std::dynamic_pointer_cast<NMinedReport>(er)) {
          NMinedReportPtr nr =
              std::dynamic_pointer_cast<NMinedReport>(er);
          line.push_back(std::to_string(nr->_n_mined_assertions));

          strategyToBestUseCase[er->_with_strategy].addUseCase(
              usecase_id, (double)nr->_n_mined_assertions, false);
        } else {
          messageError("Unknown report type");
        }

      } //end of reports
      table_rows.push_back(line);
      appendCSVLineToFile(summaryReportDumpPath, line);
    } // end of useCaseToEvalReports

    //tranform the table into a heatmap
    auto colorized_table_rows =
        processTable(table_rows, heatmap_configuration_col, 2);

    // print the table to the console
    for (size_t i = 0; i < table_rows.size(); ++i) {
      auto line = colorized_table_rows[i];
      //mark the header
      if (i == 0 || i == 1) {
        table << fort::header;
      }
      table.range_write_ln(std::begin(line), std::end(line));
    }
    std::cout << table.to_string() << std::endl;

    //print the best use case
    //for (const auto &[strategy, best] : strategyToBestUseCase) {
    //  std::string best_use_cases = "";
    //  for (const auto &use_case : best._same_value_use_cases) {
    //    best_use_cases += use_case + " ";
    //  }
    //  messageInfo("Best " + strategy + ": " + best_use_cases);
    //}

  } //end of tests
}

} // namespace usmt
