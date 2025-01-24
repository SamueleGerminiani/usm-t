#include "Test.hh"
#include "message.hh"
#include "misc.hh"
#include <filesystem>
#include <sstream>
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
      messageError("Error while creating directory '" +
                   target_input_folder + "'");
    }
  }

  std::string adapt_input_command = "bash " + ph.input_adaptor_path;
  //add the path to the input and output file of the input adaptor
  adapt_input_command += " " + original_input_folder;
  adapt_input_command += " " + target_input_folder;

  systemCheckExit(adapt_input_command);
}

void adaptOutput(const UseCase &use_case) {
  const std::string VARIABLES_MAP_FILE = getenv("VARIABLES_MAP_FILE");
  const std::string MINED_ASSERTIONS_FILE =
      getenv("MINED_ASSERTIONS_FILE");

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

  std::string adapt_output_command = "bash " + ph.output_adaptor_path;
  //add the path to the input and output file of the output adaptor (add an optional var map file)
  //in
  adapt_output_command +=
      " " + ph.work_path + ph.work_output + MINED_ASSERTIONS_FILE;
  //out
  adapt_output_command += " " + adapted_output_folder + MINED_ASSERTIONS_FILE;
  //add the path to the variables map file
  adapt_output_command +=
      " " + ph.work_path + ph.work_input + VARIABLES_MAP_FILE;

  systemCheckExit(adapt_output_command);
}
} // namespace usmt
