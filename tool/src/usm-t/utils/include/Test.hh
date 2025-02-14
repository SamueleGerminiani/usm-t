#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "UseCasePathHandler.hh"

namespace harm {
class Trace;
using TracePtr = std::shared_ptr<Trace>;
} // namespace harm

namespace rapidxml {
template <class Ch> class xml_node;
using XmlNode = rapidxml::xml_node<char>;
} // namespace rapidxml

namespace usmt {

struct TraceInput {
  std::set<std::string> paths;
  harm::TracePtr trace;
};
struct DesignInput {
  std::set<std::string> paths;
};

struct VCDInput : public TraceInput {
  std::string rst;
  std::string clk;
  std::string scope;
  using TraceInput::paths;
  using TraceInput::trace;
};

struct CSVInput : public TraceInput {
  usmt::UseCasePathHandler ph;
  using TraceInput::paths;
  using TraceInput::trace;
};

struct VerilogInput : public DesignInput {
  using DesignInput::paths;
};

struct Input {
  Input() = default;
  bool vcdExists() const {
    return std::get<0>(variants).trace != nullptr;
  }
  bool csvExists() const {
    return std::get<1>(variants).trace != nullptr;
  }
  bool verilogExists() const {
    return !std::get<2>(variants).paths.empty();
  }

  std::string getClk() const {
    if (vcdExists()) {
      return std::get<0>(variants).clk;
    }
    return "";
  }
  std::string getScope() const {
    if (vcdExists()) {
      return std::get<0>(variants).scope;
    }
    return "";
  }
  std::string getRst() const {
    if (vcdExists()) {
      return std::get<0>(variants).rst;
    }
    return "";
  }
  std::set<std::string> getVCDPaths() const {
    if (vcdExists()) {
      return std::get<0>(variants).paths;
    }
    return {};
  }
  std::set<std::string> getCSVPaths() const {
    if (csvExists()) {
      return std::get<1>(variants).paths;
    }
    return {};
  }

  std::set<std::string> getVerilogPaths() const {
    if (verilogExists()) {
      return std::get<2>(variants).paths;
    }
    return {};
  }

  harm::TracePtr getTrace() const {
    if (vcdExists()) {
      return std::get<0>(variants).trace;
    }
    if (csvExists()) {
      return std::get<1>(variants).trace;
    }
    return nullptr;
  }

  VCDInput getVCD() const { return std::get<0>(variants); }
  CSVInput getCSV() const { return std::get<1>(variants); }
  VerilogInput getVerilog() const { return std::get<2>(variants); }

  std::tuple<VCDInput, CSVInput, VerilogInput> variants;
  std::string id;
  std::set<std::string> selected_type;
  std::string dest_dir;
  rapidxml::XmlNode *xml_input;
};

struct Config {
  std::string type;
  std::string path;
};

struct ExportedVariable {
  ExportedVariable() = default;
  ExportedVariable(std::string name, std::string value)
      : name(name), value(value) {}
  std::string name;
  std::string value;
  bool operator<(const ExportedVariable &rhs) const {
    return name < rhs.name;
  }
};

struct UseCase {
  std::string usecase_id;
  std::string miner_name;
  Input input;
  std::vector<Config> configs;
  std::string input_adaptor_path;
  std::string output_adaptor_path;
  std::set<ExportedVariable> exports;
  UseCasePathHandler ph;

  //used only in external use cases
  std::string external_spec_file_path;

  rapidxml::XmlNode *xml_usecase;
  std::string docker_image = "";
};

struct Comparator {
  std::string with_strategy;
  std::string expected;
  std::string faulty_traces;
  std::string trace_type;
  std::string scope;
  std::string clk;
};

struct Test {
  std::vector<UseCase> use_cases;
  std::vector<Comparator> comparators;
  std::string mode;
  std::string name;

  rapidxml::XmlNode *xml_test;
};
} // namespace usmt
