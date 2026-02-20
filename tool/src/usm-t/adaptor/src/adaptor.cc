#include "Test.hh"
#include "adaptor_utils.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include <exception>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace usmt {
void adaptInput(const UseCase &use_case) {
  const UseCasePathHandler &ph = use_case.ph;
  messageInfo("Adapting input...");
  auto input = use_case.input;

  std::string original_input_folder =
      ph.work_path + ph.work_original_input;

  if (input.selected_type.count("csv")) {
    for (auto path : input.getCSV().paths) {
      std::filesystem::copy(path, original_input_folder);
    }
  }
  if (input.selected_type.count("vcd")) {
    for (auto path : input.getVCD().paths) {
      std::filesystem::copy(path, original_input_folder);
    }
  }

  std::string target_input_folder =
      ph.work_path + ph.work_input + input.dest_dir;

  //source design files are not adapted
  if (input.selected_type.count("verilog")) {
    for (auto path : input.getVerilog().paths) {
      std::filesystem::copy(path, target_input_folder);
    }
  }

  //create the dest input folder if it does not exist
  if (!std::filesystem::exists(target_input_folder)) {
    if (!std::filesystem::create_directory(target_input_folder)) {
      messageError("(Adaptor) Error while creating directory '" +
                   target_input_folder + "'");
    }
  }

  if (use_case.input.trim_csv_to != (size_t)-1 &&
      input.selected_type.count("csv")) {
    messageInfo("Trimming csv files to " +
                std::to_string(use_case.input.trim_csv_to) +
                " lines...");
    //make a trimmed_traces folder in the original input folder
    std::string trimmed_traces_folder =
        original_input_folder + "trimmed_traces/";
    //make directory
    messageErrorIf(
        !std::filesystem::create_directory(trimmed_traces_folder),
        "(Adaptor) Error while creating directory '" +
            trimmed_traces_folder + "'");
    copyCSV_trim_length(original_input_folder, trimmed_traces_folder,
                        use_case.input.trim_csv_to);
    original_input_folder = trimmed_traces_folder;
  }

  std::string adapt_input_command = "bash " + ph.input_adaptor_path;
  //add the path to the input and output file of the input adaptor
  adapt_input_command += " " + original_input_folder;
  adapt_input_command += " " + target_input_folder;

  systemCheckExit(adapt_input_command);

  //check that the target input folder is not empty
  messageErrorIf(std::filesystem::is_empty(target_input_folder),
                 "(Adaptor) Adapted input folder '" +
                     target_input_folder +
                     "' is empty. Check the input adaptor that was "
                     "run with commad '" +
                     adapt_input_command + "'");
}

void adaptOutput(const UseCase &use_case) {
  const std::string VARIABLES_MAP_FILE = getenv("VARIABLES_MAP_FILE");
  const std::string MINED_SPECIFICATIONS_FILE =
      getenv("MINED_SPECIFICATIONS_FILE");

  const UseCasePathHandler &ph = use_case.ph;
  messageInfo("Adapting output...");
  //create the adapter output folder
  messageErrorIf(!std::filesystem::exists(ph.work_path),
                 "Run folder '" + ph.work_path + "' does not exist");
  std::string adapted_output_folder = ph.work_path + ph.work_adapted;
  if (!std::filesystem::create_directory(adapted_output_folder)) {
    messageError("Error while creating directory '" +
                 adapted_output_folder + "'");
  }

  //check that the mined specifications file exists
  if (!std::filesystem::exists(ph.work_path + ph.work_output +
                               MINED_SPECIFICATIONS_FILE)) {
    //trow a catchable error with a message that the mined specifications file does not exist and to check that the miner was run and that the path to the mined specifications file is correct
    throw std::runtime_error(
        "Mined specifications file '" + ph.work_path +
        ph.work_output + MINED_SPECIFICATIONS_FILE +
        "' does not exist. Check that the miner was run correctly");
  }

  //check that the specifications file is not empty
  if (std::filesystem::is_empty(ph.work_path + ph.work_output +
                                MINED_SPECIFICATIONS_FILE)) {
    throw std::runtime_error(
        "Mined specifications file '" + ph.work_path +
        ph.work_output + MINED_SPECIFICATIONS_FILE +
        "' is empty. Check that the miner was run correctly");
  }

  messageInfo("Shuffling mined specifications...");
  systemCheckExit("shuf " + ph.work_path + ph.work_output +
                  MINED_SPECIFICATIONS_FILE + " -o " + ph.work_path +
                  ph.work_output + "shuffled_" +
                  MINED_SPECIFICATIONS_FILE);

  std::string adapt_output_command = "bash " + ph.output_adaptor_path;
  //add the path to the input and output file of the output adaptor (add an optional var map file)
  adapt_output_command += " " + ph.work_path + ph.work_output +
                          "shuffled_" + MINED_SPECIFICATIONS_FILE;
  //out
  adapt_output_command +=
      " " + adapted_output_folder + MINED_SPECIFICATIONS_FILE;
  //add the path to the variables map file
  adapt_output_command +=
      " " + ph.work_path + ph.work_input + VARIABLES_MAP_FILE;

  systemCheckExit(adapt_output_command);

  if (clc::maxMined != (size_t)-1) {
    messageInfo("Selecting up to " + std::to_string(clc::maxMined) +
                " mined specifications...");
    systemCheckExit("sed -i \'" + std::to_string(clc::maxMined + 1) +
                    ",$d\'" + " " + adapted_output_folder +
                    MINED_SPECIFICATIONS_FILE);
  }
  return;
}

} // namespace usmt
