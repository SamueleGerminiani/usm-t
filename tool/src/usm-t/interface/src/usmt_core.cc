#include "usmt_core.hh"
#include "EvalReport.hh"
#include "adaptor.hh"
#include "assertion_utils.hh"
#include "dump_utils.hh"
#include "globals.hh"
#include "misc.hh"
#include "test_reader.hh"
#include "usmt_evaluator.hh"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace usmt {

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

      std::unordered_map<std::string, std::vector<harm::AssertionPtr>>
          assertions;

      //find the absolute path for all paths
      initPathHandler(use_case);

      const UseCasePathHandler &ph = use_case.ph;
      bool external = use_case.external_spec_file_path != "";
      TemporalReportPtr tr = std::make_shared<TemporalReport>();

      if (!external) {
        //-------------[ADAPT THE INPUT]----------------------
        adaptInput(use_case);

        //-------------[PREPARE THE MINER]-------------------------------
        //count run number
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
          messageWarning("Miner was run with command '" +
                         run_container_command + "'");
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
      }

      if (external) {
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
      try {
        adaptOutput(use_case);
      } catch (std::exception &e) {
        messageWarning("Output adaptation failed for use case '" +
                       use_case.usecase_id +
                       "': " + std::string(e.what()));
        messageWarning(use_case.usecase_id + " will not be used");
        if (askToContinue()) {
          //skip to the next use case but dump the standalone config
          goto dump_standalone_config;
        } else {
          messageError("Aborting...");
        }
      }

      //Parse the assertions
      try {
        assertions = getExpectedMinedAssertions(
            use_case, use_case.ph.ustm_root + "/" + test.expected);
      } catch (std::exception &e) {
        messageWarning("Could not parse expected mined "
                       "assertions for use case '" +
                       use_case.usecase_id +
                       "': " + std::string(e.what()));
        messageWarning(use_case.usecase_id + " will not be used");
        if (askToContinue()) {
          //skip to the next use case but dump the standalone config
          goto dump_standalone_config;
        } else {
          messageError("Aborting...");
        }
      }

      //-----------------[EVAL]------------------------------
      for (const auto &comp : test.comparators) {
        //skip time_to_mine comparator which is implicit
        if (comp.with_strategy == "time_to_mine") {
          useCaseToEvalReports[use_case.usecase_id].push_back(tr);
          continue;
        }
        try {
          EvalReportPtr er = evaluate(use_case, comp, assertions);
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
