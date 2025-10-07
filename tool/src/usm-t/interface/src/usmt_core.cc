#include "usmt_core.hh"
#include "EvalReport.hh"
#include "Test.hh"
#include "adaptor.hh"
#include "fort.h"
#include "fort.hpp"
#include "globals.hh"
#include "misc.hh"
#include "test_reader.hh"
#include "usmt_evaluator.hh"
#include "xmlUtils.hh"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <unordered_set>
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

void dumpRadarChart(
    const std::string &radarChartDumpPath,
    const std::map<std::string, std::vector<EvalReportPtr>>
        &useCaseToEvalReports,
    const std::string &relative, const std::string &inverted,
    const std::string &title) {

  std::string usmt_root = getenv("USMT_ROOT");
  messageErrorIf(usmt_root == "",
                 "USMT_ROOT environment variable not set");
  std::string radar_chart_plotter_path =
      usmt_root + "/tool/third_party/radarChartPlotter/prc.py";
  messageErrorIf(!std::filesystem::exists(radar_chart_plotter_path),
                 "Radar chart plotter '" + radar_chart_plotter_path +
                     "' not found");

  std::vector<std::string> wanted_reports = {
      "hybrid_similarity", "fault_coverage", "time_to_mine"};
  std::string data_string = "name, MS, RS, FC, Time \n";

  //check that all use cases contain the wanted reports, if not remove them from the wanted reports list
  for (auto &wanted_report : wanted_reports) {
    for (auto &[use_case, reports] : useCaseToEvalReports) {
      if (std::find_if(reports.begin(), reports.end(),
                       [&wanted_report](const EvalReportPtr &er) {
                         return er->_with_strategy == wanted_report;
                       }) == reports.end()) {
        messageWarning("Use case '" + use_case +
                       "' does not contain report '" + wanted_report +
                       "', removing it from the radar chart");
        wanted_report = "";
        break;
      }
    }
  }
  wanted_reports.erase(
      std::remove(wanted_reports.begin(), wanted_reports.end(), ""),
      wanted_reports.end());
  if (wanted_reports.empty()) {
    messageWarning("No reports available for the radar chart, can't "
                   "generate it");
    return;
  }
  //check if hybrid_similarity is present
  if (std::find(wanted_reports.begin(), wanted_reports.end(),
                "hybrid_similarity") == wanted_reports.end()) {
    messageWarning(
        "Hybrid similarity report is not present, radar chart "
        "will not be generated");
    return;
  }

  //Make the csv string
  for (auto &[use_case, reports] : useCaseToEvalReports) {
    data_string += use_case;
    for (const auto &wanted_report : wanted_reports) {
      auto report =
          std::find_if(reports.begin(), reports.end(),
                       [&wanted_report](const EvalReportPtr &er) {
                         return er->_with_strategy == wanted_report;
                       });
      //need to handle the various report types
      if (report != reports.end()) {
        if (std::dynamic_pointer_cast<FaultCoverageReport>(*report)) {
          FaultCoverageReportPtr fcr =
              std::dynamic_pointer_cast<FaultCoverageReport>(*report);
          data_string +=
              ", " + to_string_with_precision(fcr->fault_coverage, 2);
        } else if (std::dynamic_pointer_cast<HybridReport>(*report)) {
          HybridReportPtr evmr =
              std::dynamic_pointer_cast<HybridReport>(*report);
          data_string +=
              ", " + to_string_with_precision(evmr->_final_score, 2) +
              ", " + to_string_with_precision(evmr->_noise, 2);
        } else if (std::dynamic_pointer_cast<TemporalReport>(
                       *report)) {
          TemporalReportPtr tr =
              std::dynamic_pointer_cast<TemporalReport>(*report);
          data_string += ", " + to_string_with_precision(
                                    (double)tr->_timeMS / 1000.f, 2);
        } else if (std::dynamic_pointer_cast<NMinedReport>(*report)) {
          NMinedReportPtr nr =
              std::dynamic_pointer_cast<NMinedReport>(*report);
          data_string +=
              ", " + std::to_string(nr->_n_mined_assertions);
        } else {
          messageError("Unknown report type");
        }
      } else {
        messageWarning("Use case '" + use_case +
                       "' does not contain report '" + wanted_report +
                       "', skipping it");
      }
    }
    data_string += " \n";
  }

  //debug
  //std::cout << "Title: " << title << "\n";
  //std::cout << "Dump to: " << radarChartDumpPath << "\n";
  //std::cout << "Using tool: " << radar_chart_plotter_path << "\n";
  //std::cout << "Relative: " << relative << "\n";
  //std::cout << "Inverted: " << inverted << "\n";
  //std::cout << "Data string:\n" << data_string << "\n";

  if (systemCheckFailure(
          "python3 " + radar_chart_plotter_path + " --dump-to \"" +
          radarChartDumpPath + "\"" + " --relative \"" + relative +
          "\" --invert \"" + inverted + "\" --title \"" + title +
          "\"" + " --string \"" + data_string + "\"")) {

    messageWarning("Could not generate radar chart");
  } else {
    messageInfo("Radar chart dumped to '" + radarChartDumpPath + "'");
  }
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

std::vector<std::string> makeHeader(fort::utf8_table &table,
                                    const Test &test) {
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
  return line_header;
}

std::vector<std::string>
makeSubHeader(fort::utf8_table &table, const Test &test,
              std::vector<size_t> &heatmap_configuration_col) {
  std::vector<std::string> line_subheader;
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
  return line_subheader;
}

void makeReportEntry(
    const EvalReportPtr &er, std::vector<std::string> &line,
    const std::string &usecase_id,
    std::map<std::string, BestUseCase> &strategyToBestUseCase) {

  if (std::dynamic_pointer_cast<FaultCoverageReport>(er)) {
    FaultCoverageReportPtr fcr =
        std::dynamic_pointer_cast<FaultCoverageReport>(er);
    line.push_back(to_string_with_precision(fcr->fault_coverage, 2));
    line.push_back(
        std::to_string(fcr->_minCoveringAssertions.size()));
    strategyToBestUseCase[er->_with_strategy].addUseCase(
        usecase_id, fcr->fault_coverage);
  } else if (std::dynamic_pointer_cast<SemanticEquivalenceReport>(
                 er)) {
    SemanticEquivalenceReportPtr evmr =
        std::dynamic_pointer_cast<SemanticEquivalenceReport>(er);
    line.push_back(to_string_with_precision(evmr->_final_score, 2));
    line.push_back(to_string_with_precision(evmr->_noise, 2));
    strategyToBestUseCase[er->_with_strategy].addUseCase(
        usecase_id, evmr->_final_score);
  } else if (std::dynamic_pointer_cast<HybridReport>(er)) {
    HybridReportPtr evmr =
        std::dynamic_pointer_cast<HybridReport>(er);
    line.push_back(to_string_with_precision(evmr->_final_score, 2));
    line.push_back(to_string_with_precision(evmr->_noise, 2));
    strategyToBestUseCase[er->_with_strategy].addUseCase(
        usecase_id, evmr->_final_score);
  } else if (std::dynamic_pointer_cast<SyntacticSimilarityReport>(
                 er)) {
    SyntacticSimilarityReportPtr evmr =
        std::dynamic_pointer_cast<SyntacticSimilarityReport>(er);
    line.push_back(to_string_with_precision(evmr->_final_score, 2));
    line.push_back(to_string_with_precision(evmr->_noise, 2));
    strategyToBestUseCase[er->_with_strategy].addUseCase(
        usecase_id, evmr->_final_score);
  } else if (std::dynamic_pointer_cast<TemporalReport>(er)) {
    TemporalReportPtr tr =
        std::dynamic_pointer_cast<TemporalReport>(er);
    line.push_back(
        to_string_with_precision((double)tr->_timeMS / 1000.f, 2) +
        "s");

    strategyToBestUseCase[er->_with_strategy].addUseCase(
        usecase_id, (double)tr->_timeMS / 1000.f, false);
  } else if (std::dynamic_pointer_cast<NMinedReport>(er)) {
    NMinedReportPtr nr = std::dynamic_pointer_cast<NMinedReport>(er);
    line.push_back(std::to_string(nr->_n_mined_assertions));

    strategyToBestUseCase[er->_with_strategy].addUseCase(
        usecase_id, (double)nr->_n_mined_assertions, false);
  } else {
    messageError("Unknown report type");
  }
}
bool askToContinue() {
  if (clc::continueOnError) {
    return true;
  }
  std::cout << "Do you want to continue? [y/n] ";
  std::string answer;
  std::getline(std::cin, answer);
  if (answer == "y" || answer == "Y" || answer == "") {
    return true;
  } else {
    return false;
  }
}

void run_usmt() {

  std::vector<Test> tests = readTestFile(clc::testFile);

  for (auto &test : tests) {
    std::vector<std::vector<std::string>> table_rows;
    std::vector<size_t> heatmap_configuration_col = {0};

    messageInfo("Running test " + test.name);
    std::string summaryReportDumpPath = "";
    std::string radarChartDumpPath = "";
    if (clc::dumpPath != "") {
      summaryReportDumpPath =
          clc::dumpPath + "/summary_report_" + test.name + ".csv";
      radarChartDumpPath =
          clc::dumpPath + "/radar_chart_" + test.name + ".png";
      //remove previous report
      if (std::filesystem::exists(summaryReportDumpPath)) {
        std::filesystem::remove(summaryReportDumpPath);
      }
      //remove previous radar chart
      if (std::filesystem::exists(radarChartDumpPath)) {
        std::filesystem::remove(radarChartDumpPath);
      }
    }

    fort::utf8_table table;
    table.set_border_style(FT_PLAIN_STYLE);

    //handle the header of the table
    std::vector<std::string> line_header = makeHeader(table, test);
    table_rows.push_back(line_header);
    if (summaryReportDumpPath != "")
      appendCSVLineToFile(summaryReportDumpPath, line_header);

    //handle the subheader of the table
    std::vector<std::string> line_subheader =
        makeSubHeader(table, test, heatmap_configuration_col);
    table << fort::header;
    table_rows.push_back(line_subheader);
    if (summaryReportDumpPath != "")
      appendCSVLineToFile(summaryReportDumpPath, line_subheader);

    std::map<std::string, std::vector<EvalReportPtr>>
        useCaseToEvalReports;
    for (auto &use_case : test.use_cases) {
      //find the absolute path for all paths
      initPathHandler(use_case);

      const UseCasePathHandler &ph = use_case.ph;
      bool external = use_case.external_spec_file_path != "";
      TemporalReportPtr tr = std::make_shared<TemporalReport>();

      if (!external) {
        //-------------[ADAPT THE INPUT]----------------------
        adaptInput(use_case);

        //-------------[PREPARE THE MINER]-------------------------------
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

        //-----------------[RUN THE MINER]---------------------
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

        auto start = std::chrono::high_resolution_clock::now();
        if (systemCheckFailure(run_container_command)) {
          messageWarning("Miner '" + use_case.miner_name +
                         "' failed");
          if (askToContinue()) {
            //skip to the next use case but dump the standalone config
            goto dump_standalone_config;
          } else {
            messageError("Aborting...");
          }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        //keep track of the elapsed time
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

      //-----------------[ADAPT THE OUTPUT]------------------
      adaptOutput(use_case);

      //-----------------[EVAL]------------------------------
      for (const auto &comp : test.comparators) {
        //skip time_to_mine comparator which is implicit
        if (comp.with_strategy == "time_to_mine") {
          useCaseToEvalReports[use_case.usecase_id].push_back(tr);
          continue;
        }
        try {
          EvalReportPtr er = evaluate(use_case, comp);
          std::cout << er->to_string() << "\n";
          er->dumpTo(ph.work_path + ph.work_eval);
          useCaseToEvalReports[use_case.usecase_id].push_back(er);
        } catch (std::exception &e) {
          messageWarning("Evaluation " + comp.with_strategy +
                         " for use case '" + use_case.usecase_id +
                         "' failed: " + std::string(e.what()));
          messageWarning(use_case.usecase_id + " will not be used");
          if (askToContinue()) {
            //skip to the next use case but dump the standalone config
            useCaseToEvalReports.erase(use_case.usecase_id);
            goto dump_standalone_config;
          } else {
            messageError("Aborting...");
          }
        }
      }

      //------------[DUMP THE STANDALONE CONFIG] ---------
    dump_standalone_config:;
      dumpConfigStandalone(use_case, test,
                           ph.work_path +
                               ph.work_test_config_standalone +
                               "config.xml");
      messageInfo("Dumping standalone test to '" + ph.work_path +
                  ph.work_test_config_standalone + "config.xml'");

    } //end of use cases

    //report to best use case
    std::map<std::string, BestUseCase> strategyToBestUseCase;

    //-------------[AGGREGATE THE EVALUATION REPORTS]-----
    for (const auto &[usecase_id, report] : useCaseToEvalReports) {
      std::vector<std::string> line;
      line.push_back(usecase_id);
      for (const auto &er : report) {
        makeReportEntry(er, line, usecase_id, strategyToBestUseCase);
      }
      table_rows.push_back(line);
      if (summaryReportDumpPath != "")
        appendCSVLineToFile(summaryReportDumpPath, line);
    }

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

    //FIXME: print the best use case, does this even work?
    //for (const auto &[strategy, best] : strategyToBestUseCase) {
    //  std::string best_use_cases = "";
    //  for (const auto &use_case : best._same_value_use_cases) {
    //    best_use_cases += use_case + " ";
    //  }
    //  messageInfo("Best " + strategy + ": " + best_use_cases);
    //}

    if (radarChartDumpPath != "")
      dumpRadarChart(radarChartDumpPath, useCaseToEvalReports, "Time",
                     "Time, RS", test.name);
  } //end of tests
}

} // namespace usmt
