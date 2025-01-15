#pragma once
#include <string>

namespace usmt {
class UseCase;

class UseCasePathHandler {
public:
  virtual ~UseCasePathHandler() = default;
  UseCasePathHandler() = default;

  std::string ustm_root = "";
  std::string miners_path = "";
  std::string miner_path = "";
  std::string tools_path = "";
  std::string work_input = "";
  std::string work_output = "";
  std::string work_eval = "";
  std::string work_path = "";
  std::string run_container_path = "";

  std::string input_adaptor_path = "";
  std::string output_adaptor_path = "";
  std::string configurations_path = "";
};

std::string getCurrentDateTime();

void initPathHandler(UseCase &us);

} // namespace usmt

