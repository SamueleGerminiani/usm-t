#include "dump_utils.hh"
#include "misc.hh"
#include "xmlUtils.hh"
#include <algorithm>
#include <filesystem>

namespace usmt {

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
  std::string data_string = "name, MS, RS, SFC, Time \n";

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
  std::ofstream debug_file("radar_chart_debug_" + title + ".csv");
  debug_file << data_string;
  debug_file.close();

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
             size_t skip_top_rows) {

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
      line_subheader.push_back("SFC");
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
} // namespace usmt
