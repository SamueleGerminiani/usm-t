#pragma once
#include "EvalReport.hh"
#include "Test.hh"
#include "fort.h"
#include "fort.hpp"
#include <map>
#include <string>
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
                         const std::vector<std::string> &line);

void dumpRadarChart(
    const std::string &radarChartDumpPath,
    const std::map<std::string, std::vector<EvalReportPtr>>
        &useCaseToEvalReports,
    const std::string &relative, const std::string &inverted,
    const std::string &title);

// Function to convert numerical value to a colored string based on min and max values
std::string colorize(double value, double min, double max,
                     bool reverse);

std::vector<std::vector<std::string>>
processTable(const std::vector<std::vector<std::string>> &table_rows,
             const std::vector<size_t> &heatmap_configuration_col,
             size_t skip_top_rows = 0);

void dumpConfigStandalone(const UseCase &uc, const Test &test,
                          std::string dump_file);

std::vector<std::string> makeHeader(fort::utf8_table &table,
                                    const Test &test);

std::vector<std::string>
makeSubHeader(fort::utf8_table &table, const Test &test,
              std::vector<size_t> &heatmap_configuration_col);

void makeReportEntry(
    const EvalReportPtr &er, std::vector<std::string> &line,
    const std::string &usecase_id,
    std::map<std::string, BestUseCase> &strategyToBestUseCase);

} // namespace usmt
