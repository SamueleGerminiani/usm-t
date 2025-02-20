#include "test_reader.hh"
#include "CSVtraceReader.hh"
#include "PointerUtils.hh"
#include "Test.hh"
#include "Trace.hh"
#include "VCDtraceReader.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "rapidxml.hh"
#include "xmlUtils.hh"
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace usmt {
using namespace rapidxml;
using namespace harm;

std::vector<Test> parseTests(XmlNode *root);

std::vector<Test> readTestFile(const std::string &filename) {

  try {
    rapidxml::file<> *xmlFile =
        new rapidxml::file<>(filename.c_str());
    rapidxml::xml_document<> *doc = new rapidxml::xml_document<>();
    doc->parse<0>(xmlFile->data());

    XmlNode *root = doc->first_node("usm-t");
    if (root) {
      return parseTests(root);
    } else {
      messageError("Failed to find root 'usm-t'");
    }
  } catch (rapidxml::parse_error &e) {
    std::stringstream ss;
    ss << "Parsing error in " << filename << " "
       << "\n"
       << e.m_what << " at line " << e.m_lineNumber << std::endl;
    if (!reinterpret_cast<char *>(e.m_where)[0]) {
      ss << "Hint: you probably forgot to close a tag\n";
    }
    messageError(ss.str());
  }
  return {};
}

std::vector<std::string>
recoverFilesInDirectoryWithExtension(const std::string &path,
                                     const std::string &extension) {
  messageErrorIf(!std::filesystem::exists(path),
                 "Could not find '" + path + "'");
  messageErrorIf(!std::filesystem::is_directory(path),
                 "Not a directory: '" + path + "'");
  std::vector<std::string> ret;
  for (const auto &entry :
       std::filesystem::directory_iterator(path)) {

    if (entry.path().extension() == extension) {
      ret.push_back(entry.path().u8string());
    }
  }
  return ret;
}

Input parseInput(XmlNode *inputNode) {
  messageErrorIf(getenv("USMT_ROOT") == nullptr,
                 "USTM_ROOT environment variable not set");

  std::string ustm_root = getenv("USMT_ROOT");
  std::string input_prefix = ustm_root + "/input/";

  std::string id = getAttributeValue(inputNode, "id", "");
  messageErrorIf(id == "", "Input id cannot be empty in 'input' tag");

  //VCD input
  std::vector<rapidxml::xml_node<> *> vcdNodes;
  getNodesFromName(inputNode, "vcd", vcdNodes);
  std::vector<rapidxml::xml_node<> *> vcddirNodes;
  getNodesFromName(inputNode, "vcd_dir", vcddirNodes);
  std::vector<rapidxml::xml_node<> *> allVCDNodes;
  allVCDNodes.insert(allVCDNodes.end(), vcdNodes.begin(),
                     vcdNodes.end());
  allVCDNodes.insert(allVCDNodes.end(), vcddirNodes.begin(),
                     vcddirNodes.end());

  VCDInput vcd_in;
  std::vector<std::string> allVCDTraces;

  //gather all vcd paths
  if (!allVCDNodes.empty()) {
    for (auto tn : allVCDNodes) {
      std::string path =
          input_prefix + getAttributeValue(tn, "path", "");
      messageErrorIf(path.empty(),
                     "Path cannot be empty in 'input' tag");
      std::string clk = getAttributeValue(tn, "clk", "");
      messageErrorIf(clk == "",
                     "VCD input '" + id +
                         "' must have a clk in the xml tag");
      messageErrorIf(vcd_in.clk != "" && clk != vcd_in.clk,
                     "VCD input '" + id +
                         "' must have the same clk for all vcd "
                         "traces in the same input set");
      vcd_in.clk = clk;

      std::string rst = getAttributeValue(tn, "rst", "");
      messageErrorIf(vcd_in.rst != "" && rst != vcd_in.rst,
                     "VCD input '" + id +
                         "' must have the same rst for all vcd "
                         "traces in the same input set");
      vcd_in.rst = rst;

      std::string scope = getAttributeValue(tn, "scope", "");
      messageErrorIf(scope == "",
                     "VCD input '" + id +
                         "' must have a scope in the xml tag");
      messageErrorIf(vcd_in.scope != "" && scope != vcd_in.scope,
                     "VCD input '" + id +
                         "' must have the same scope for all vcd "
                         "traces in the same input set");
      vcd_in.scope = scope;

      messageErrorIf(vcd_in.paths.count(path),
                     "VCD input '" + id +
                         "' cannot have repeated paths");
      if (std::filesystem::is_directory(path)) {
        auto traces_in_dir =
            recoverFilesInDirectoryWithExtension(path, ".vcd");
        allVCDTraces.insert(allVCDTraces.end(), traces_in_dir.begin(),
                            traces_in_dir.end());

      } else {
        allVCDTraces.push_back(path);
      }
    }
  }

  if (!allVCDTraces.empty()) {
    std::sort(allVCDTraces.begin(), allVCDTraces.end());
    VCDTraceReaderConfig vcd_config;
    vcd_config._clk = vcd_in.clk;
    vcd_config._selectedScope = vcd_in.scope;
    vcd_config._vcdRecursive = 0;

    TraceReaderPtr tr =
        generatePtr<VCDtraceReader>(allVCDTraces, vcd_config);
    vcd_in.trace = tr->readTrace();
    for (auto trace : allVCDTraces) {
      vcd_in.paths.insert(trace);
    }
  }

  //CSV input
  std::vector<rapidxml::xml_node<> *> csvNodes;
  getNodesFromName(inputNode, "csv", csvNodes);
  std::vector<rapidxml::xml_node<> *> csvdirNodes;
  getNodesFromName(inputNode, "csv_dir", csvdirNodes);
  std::vector<rapidxml::xml_node<> *> allCSVNodes;
  allCSVNodes.insert(allCSVNodes.end(), csvNodes.begin(),
                     csvNodes.end());
  allCSVNodes.insert(allCSVNodes.end(), csvdirNodes.begin(),
                     csvdirNodes.end());

  CSVInput csv_in;
  std::vector<std::string> allCSVTraces;

  //gather all csv paths
  if (!allCSVNodes.empty()) {
    for (auto tn : allCSVNodes) {
      std::string path =
          input_prefix + getAttributeValue(tn, "path", "");
      messageErrorIf(path.empty(),
                     "Path cannot be empty in 'input' tag");

      messageErrorIf(csv_in.paths.count(path),
                     "CSV input '" + id +
                         "' cannot have repeated paths");
      if (std::filesystem::is_directory(path)) {
        auto traces_in_dir =
            recoverFilesInDirectoryWithExtension(path, ".csv");
        allCSVTraces.insert(allCSVTraces.end(), traces_in_dir.begin(),
                            traces_in_dir.end());

      } else {
        allCSVTraces.push_back(path);
      }
    }
  }

  if (!allCSVTraces.empty()) {
    std::sort(allCSVTraces.begin(), allCSVTraces.end());
    TraceReaderPtr tr = generatePtr<CSVtraceReader>(allCSVTraces);
    csv_in.trace = tr->readTrace();

    for (auto trace : allCSVTraces) {
      csv_in.paths.insert(trace);
    }
  }

  //Verilog input
  std::vector<rapidxml::xml_node<> *> verilogNodes;
  getNodesFromName(inputNode, "verilog", verilogNodes);
  std::vector<rapidxml::xml_node<> *> verilogdirNodes;
  getNodesFromName(inputNode, "verilog_dir", verilogdirNodes);
  std::vector<rapidxml::xml_node<> *> allVerilogNodes;
  allVerilogNodes.insert(allVerilogNodes.end(), verilogNodes.begin(),
                         verilogNodes.end());
  allVerilogNodes.insert(allVerilogNodes.end(),
                         verilogdirNodes.begin(),
                         verilogdirNodes.end());

  VerilogInput verilog_in;

  //gather all verilog paths
  if (!allVerilogNodes.empty()) {
    for (auto tn : allVerilogNodes) {
      std::string path =
          input_prefix + getAttributeValue(tn, "path", "");
      messageErrorIf(path.empty(),
                     "Path cannot be empty in 'input' tag");

      messageErrorIf(verilog_in.paths.count(path),
                     "Verilog input '" + id +
                         "' cannot have repeated paths");
      if (std::filesystem::is_directory(path)) {
        auto traces_in_dir =
            recoverFilesInDirectoryWithExtension(path, ".v");
        for (auto trace : traces_in_dir) {
          verilog_in.paths.insert(trace);
        }
      } else {
        verilog_in.paths.insert(path);
      }
    }
    std::sort(allVCDTraces.begin(), allVCDTraces.end());
  }

  if (!csv_in.paths.empty() && !vcd_in.paths.empty()) {

    //Check that name of csv and vcd files match (without extension)
    std::vector<std::string> csv_names;
    for (auto csv_path : csv_in.paths) {
      std::string csv_name =
          csv_path.substr(csv_path.find_last_of("/\\") + 1);
      csv_name = csv_name.substr(0, csv_name.find_last_of("."));
      csv_names.push_back(csv_name);
    }
    std::vector<std::string> vcd_names;
    for (auto vcd_path : vcd_in.paths) {
      std::string vcd_name =
          vcd_path.substr(vcd_path.find_last_of("/\\") + 1);
      vcd_name = vcd_name.substr(0, vcd_name.find_last_of("."));
      vcd_names.push_back(vcd_name);
    }
    //check if the sets are equal
    messageErrorIf(
        csv_names.size() != vcd_names.size(),
        "CSV and VCD traces are not equal in input '" + id +
            "': different number of trace files found " +
            std::to_string(csv_names.size()) + " (csv) != " +
            std::to_string(vcd_names.size()) + " (vcd)");

    std::sort(csv_names.begin(), csv_names.end());
    std::sort(vcd_names.begin(), vcd_names.end());
    messageErrorIf(csv_names != vcd_names,
                   "CSV and VCD trace files dont have the same base "
                   "names in input '" +
                       id + "'");

    //we need to check that the traces are equal in content
    messageErrorIf(csv_in.trace->getLength() !=
                       vcd_in.trace->getLength(),
                   "CSV and VCD traces have different content: "
                   "different length");

    messageErrorIf(!(csv_in.trace == vcd_in.trace),
                   "CSV and VCD traces have different content: "
                   "different variables or different samples'" +
                       id + "'");
  }
  Input ret;
  ret.xml_input = inputNode;
  ret.variants = std::make_tuple(vcd_in, csv_in, verilog_in);
  ret.id = id;
  return ret;
}

void parseConfigs(XmlNode *configNode, std::vector<Config> &configs) {
  XmlNodeList configNodes;
  getNodesFromName(configNode, "configuration", configNodes);
  for (auto cfgNode : configNodes) {
    Config config;
    config.type = getAttributeValue(cfgNode, "type", "");
    config.path = getAttributeValue(cfgNode, "path", "");
    messageErrorIf(config.type.empty() || config.path.empty(),
                   "Config type and path cannot be empty");
    configs.push_back(config);
  }
}
Comparator parseCompare(XmlNode *compareNode) {

  Comparator comp;
  comp.with_strategy =
      getAttributeValue(compareNode, "with_strategy", "");
  messageErrorIf(!(comp.with_strategy == "fault_coverage" ||
                   comp.with_strategy == "semantic_equivalence" ||
                   comp.with_strategy == "hybrid_similarity" ||
                   comp.with_strategy == "syntactic_similarity" ||
                   comp.with_strategy == "n_mined" ||
                   comp.with_strategy == "time_to_mine"),
                 "Comparator strategy '" + comp.with_strategy +
                     "' not supported, supported strategies are "
                     "'fault_coverage', 'semantic_equivalence', "
                     "'hybrid_similarity', 'syntactic_similarity', "
                     "'n_mined' and "
                     "'time_to_mine'");

  comp.faulty_traces =
      getAttributeValue(compareNode, "faulty_traces", "");
  comp.trace_type = getAttributeValue(compareNode, "trace_type", "");
  if (comp.trace_type == "vcd") {
    comp.clk = getAttributeValue(compareNode, "clk", "");
    messageErrorIf(comp.clk == "",
                   "VCD trace type must have a clk in the xml tag of "
                   "a fault coverage comparator");
    comp.scope = getAttributeValue(compareNode, "scope", "");
    messageErrorIf(
        comp.scope == "",
        "VCD trace type must have a scope in the xml tag of "
        "a fault coverage comparator");
  }
  messageErrorIf(
      (comp.trace_type.empty() || comp.faulty_traces.empty()) &&
          comp.faulty_traces == "fault_coverage",
      "Must specify a path to a set of faulty traces with the "
      "attribute 'faulty_traces' and the trace type with the "
      "attribute 'trace_type' when using the 'fault_coverage' "
      "strategy");

  return comp;
}

void populateWithInput(
    UseCase &usecase, XmlNode *usecaseNode,
    const std::unordered_map<std::string, Input> &idToInput) {

  std::vector<rapidxml::xml_node<> *> inputNodes;
  getNodesFromName(usecaseNode, "input", inputNodes);
  messageErrorIf(
      inputNodes.size() > 1,
      "There should be at most one input tag in a usecase");
  messageErrorIf(
      inputNodes.size() == 0,
      "There should be at least one input tag in a usecase");
  auto inode = inputNodes[0];
  std::string id = getAttributeValue(inode, "id", "");
  messageErrorIf(id == "", "Input id cannot be empty in input tag");
  std::string type = getAttributeValue(inode, "type", "");
  messageErrorIf(type.empty(),
                 "Input type cannot be empty in input tag with id '" +
                     id + "'");
  messageErrorIf(!idToInput.count(id),
                 "Input with id '" + id + "' not found");
  std::string dest_dir = getAttributeValue(inode, "dest_dir", "");

  usecase.input = idToInput.at(id);
  usecase.input.selected_type = parseCSVToSet(type);

  for (auto type : usecase.input.selected_type) {
    messageErrorIf(type != "vcd" && type != "csv" &&
                       type != "verilog",
                   "Input type '" + type +
                       "' not supported, supported types are 'vcd', "
                       "'csv' and 'verilog'");
  }
  usecase.input.dest_dir = dest_dir;
}

UseCase parseExternal(
    XmlNode *externalNode,
    const std::unordered_map<std::string, Input> &idToInput) {
  UseCase usecase;
  usecase.xml_usecase = externalNode;
  usecase.usecase_id = getAttributeValue(externalNode, "id", "");
  messageErrorIf(usecase.usecase_id.empty(),
                 "Usecase id cannot be empty");
  usecase.miner_name = "external";

  std::vector<rapidxml::xml_node<> *> specificationsNodes;
  getNodesFromName(externalNode, "external_specifications",
                   specificationsNodes);
  //check that there is only one external_specifications tag
  messageErrorIf(specificationsNodes.size() != 1,
                 "There should be exactly one "
                 "external_specifications tag in external with id '" +
                     usecase.usecase_id + "'");
  auto specNode = specificationsNodes[0];

  usecase.external_spec_file_path =
      getAttributeValue(specNode, "path", "");
  messageErrorIf(usecase.external_spec_file_path.empty(),
                 "External specifications file path cannot be empty "
                 "in external with id '" +
                     usecase.usecase_id + "'");

  std::vector<rapidxml::xml_node<> *> output_adaptorNodes;
  getNodesFromName(externalNode, "output_adaptor",
                   output_adaptorNodes);

  usecase.output_adaptor_path =
      getAttributeValue(output_adaptorNodes[0], "path", "");
  messageErrorIf(
      usecase.output_adaptor_path.empty(),
      "Output adaptor path cannot be empty in external with id '" +
          usecase.usecase_id + "'");

  populateWithInput(usecase, externalNode, idToInput);

  return usecase;
}
UseCase parseUseCase(
    XmlNode *usecaseNode,
    const std::unordered_map<std::string, Input> &idToInput) {
  // Parse usecase
  UseCase usecase;
  usecase.xml_usecase = usecaseNode;
  usecase.usecase_id = getAttributeValue(usecaseNode, "id", "");
  messageErrorIf(usecase.usecase_id.empty(),
                 "Usecase id cannot be empty");
  // Parse miner
  std::vector<rapidxml::xml_node<> *> minerNodes;
  getNodesFromName(usecaseNode, "miner", minerNodes);
  messageErrorIf(minerNodes.size() != 1,
                 "There should be exactly one miner tag got '" +
                     std::to_string(minerNodes.size()) + "'");

  usecase.miner_name = getAttributeValue(minerNodes[0], "name", "");

  messageErrorIf(usecase.miner_name.empty(),
                 "Miner name cannot be empty");

  // Parse input
  populateWithInput(usecase, usecaseNode, idToInput);

  // Parse configs
  parseConfigs(usecaseNode, usecase.configs);

  std::vector<rapidxml::xml_node<> *> input_adaptorNodes;
  getNodesFromName(usecaseNode, "input_adaptor", input_adaptorNodes);
  messageErrorIf(input_adaptorNodes.size() != 1,
                 "There should be exactly one input_adaptor tag");

  // Parse input adaptor
  usecase.input_adaptor_path =
      getAttributeValue(input_adaptorNodes[0], "path", "");
  messageErrorIf(usecase.input_adaptor_path.empty(),
                 "Input adaptor path of "
                 "usecase with id '" +
                     usecase.usecase_id + "' cannot be empty");

  // Parse output adaptor
  std::vector<rapidxml::xml_node<> *> output_adaptorNodes;
  getNodesFromName(usecaseNode, "output_adaptor",
                   output_adaptorNodes);
  usecase.output_adaptor_path =
      getAttributeValue(output_adaptorNodes[0], "path", "");
  messageErrorIf(usecase.output_adaptor_path.empty(),
                 "Output adaptor path cannot be empty");

  std::vector<rapidxml::xml_node<> *> exportNodes;
  getNodesFromName(usecaseNode, "export", exportNodes);
  for (auto n : exportNodes) {
    auto attributes = getAttributes(n);
    if (!attributes.empty()) {
      for (auto attr : attributes) {
        usecase.exports.insert(
            ExportedVariable(attr.first, attr.second));
      }
    }
  }

  std::vector<rapidxml::xml_node<> *> dockerNodes;
  getNodesFromName(usecaseNode, "docker", dockerNodes);
  messageErrorIf(
      dockerNodes.size() != 1,
      "There should be exactly one docker tag in usecase '" +
          usecase.usecase_id + "'");
  usecase.docker_image =
      getAttributeValue(dockerNodes[0], "image", "");
  messageErrorIf(usecase.docker_image.empty(),
                 "docker image cannot be empty in usecase '" +
                     usecase.usecase_id + "'");
  //debug
  //if (!usecase.exports.empty()) {
  //  //print the exports
  //  for (auto export_ : usecase.exports) {
  //    std::cout << "Export: " << export_.name << " " << export_.value
  //              << std::endl;
  //  }
  //}
  //exit(0);

  return usecase;
}

std::vector<Test> parseTests(XmlNode *root) {

  XmlNodeList inputNodes;
  getNodesFromName(root, "input", inputNodes);
  std::unordered_map<std::string, Input> idToInput;
  for (auto inputNode : inputNodes) {
    Input input = parseInput(inputNode);
    //check for repeated input ids
    messageErrorIf(idToInput.count(input.id),
                   "Repeated input id '" + input.id + "'");
    idToInput[input.id] = input;
  }

  //parse usecases declarations
  XmlNodeList usecaseNodes;
  getNodesFromName(root, "usecase", usecaseNodes);
  std::unordered_map<std::string, UseCase> idToUseCase;
  for (auto usecaseNode : usecaseNodes) {
    auto usecase = parseUseCase(usecaseNode, idToInput);
    messageErrorIf(idToUseCase.count(usecase.usecase_id),
                   "Repeated usecase id '" + usecase.usecase_id +
                       "'");
    idToUseCase[usecase.usecase_id] = usecase;
  }

  XmlNodeList externalNodes;
  getNodesFromName(root, "external", externalNodes);
  for (auto usecaseNode : externalNodes) {
    auto external = parseExternal(usecaseNode, idToInput);
    messageErrorIf(idToUseCase.count(external.usecase_id),
                   "Repeated usecase id '" + external.usecase_id +
                       "'");
    idToUseCase[external.usecase_id] = external;
  }

  //parse tests
  std::vector<Test> tests;
  XmlNodeList testNodes;
  getNodesFromName(root, "test", testNodes);
  messageErrorIf(testNodes.size() == 0,
                 "There should be at least one test tag");

  for (auto testNode : testNodes) {
    Test test;
    test.xml_test = testNode;
    test.name = getAttributeValue(testNode, "name", "");
    //test.mode = getAttributeValue(testNode, "mode", "");
    //messageErrorIf(test.name.empty() || test.mode.empty(),
    //               "Test name or mode cannot be empty in test tag");
    messageErrorIf(test.name.empty(),
                   "Test name cannot be empty in test tag");
    //parse comparators
    XmlNodeList expectedNodes;
    getNodesFromName(testNode, "expected", expectedNodes);
    messageErrorIf(expectedNodes.size() > 1,
                   "There should be at most one expected tag");
    std::string expected = "";
    if (!expectedNodes.empty()) {
      expected = getAttributeValue(expectedNodes[0], "path", "");
    }

    //parse comparators
    XmlNodeList compareNodes;
    getNodesFromName(testNode, "compare", compareNodes);
    for (auto compareNode : compareNodes) {
      Comparator comp = parseCompare(compareNode);
      comp.expected = expected;
      messageErrorIf(
          comp.expected == "" &&
              (comp.with_strategy == "semantic_equivalence" ||
               comp.with_strategy == "systactic_similarity" ||
               comp.with_strategy == "n_mined" ||
               comp.with_strategy == "hybrid_similarity"),
          "Must specify a path to a set of golden assertions "
          "with the 'expected' tag when using the "
          "'semantic_equivalence', 'hybrid_similarity', 'n_mined' or "
          "'syntactic_similarity strategy");
      test.comparators.push_back(comp);
    }
    //error if there are repeated comparators
    std::unordered_map<std::string, int> comparatorCount;
    for (auto comp : test.comparators) {
      comparatorCount[comp.with_strategy]++;
      messageErrorIf(comparatorCount[comp.with_strategy] > 1,
                     "Repeated comparator '" + comp.with_strategy +
                         "' in test '" + test.name + "'");
    }

    messageWarningIf(test.comparators.empty(),
                     "Non comparators found in test '" + test.name +
                         "'");

    //Parse usecases and externals used in the test
    XmlNodeList usecaseNodes;
    getNodesFromName(testNode, "usecase", usecaseNodes);
    getNodesFromName(testNode, "external", usecaseNodes);
    std::string this_test_input_id = "";
    for (auto usecaseNode : usecaseNodes) {
      std::string id = getAttributeValue(usecaseNode, "id", "");
      messageErrorIf(id.empty(),
                     "Usecase id cannot be empty in test tag");
      if (idToUseCase.count(id) == 0) {
        messageWarning("Usecase with id '" + id + "' not found");
      } else {
        test.use_cases.push_back(idToUseCase[id]);
      }
      //check that all usecases in a test use the same input id
      if (this_test_input_id == "") {
        this_test_input_id = idToUseCase[id].input.id;
      } else {
        messageErrorIf(
            this_test_input_id != idToUseCase[id].input.id,
            "All usecases in a test must use the same "
            "input. In test '" +
                test.name +
                "', "
                "usecase '" +
                idToUseCase[id].usecase_id + "' uses input id '" +
                idToUseCase[id].input.id + "' while usecase '" +
                test.use_cases[0].usecase_id + "' uses input id '" +
                this_test_input_id + "'");
      }
    }

    tests.push_back(test);
  }

  //print tests
  for (auto test : tests) {
    std::cout << "TEST: " << test.name;
    std::cout << "\tCOMPARATORS:"
              << "\n";
    for (auto comp : test.comparators) {
      std::cout << "\t\t\twith_strategy: " << comp.with_strategy
                << "\n";
      if (comp.with_strategy != "fault_coverage" &&
          comp.with_strategy != "time_to_mine") {
        std::cout << "\t\t\texpected: " << comp.expected << "\n";
      }
    }

    for (auto usecase : test.use_cases) {
      std::cout << "USECASE: " << usecase.usecase_id
                << "-------------------\n";
      std::cout << "MINER: " << usecase.miner_name << std::endl;
      auto input = usecase.input;
      if (!usecase.exports.empty()) {
        //print the exports
        std::cout << "EXPORTS:"
                  << "\n";
        for (auto export_ : usecase.exports) {
          std::cout << "\t\t\t" << export_.name << " "
                    << export_.value << std::endl;
        }
      }
      std::cout << "INPUT"
                << "\n";
      std::cout << "\t\t\tID: " << input.id << "\n";
      std::cout << "\t\t\tSELECTED TYPE: ";
      for (auto type : input.selected_type) {
        std::cout << type << " ";
      }
      std::cout << "\n";
      if (input.vcdExists() && input.selected_type.count("vcd")) {
        std::cout << "VCD"
                  << "\n";
        std::cout << "\t\t\tCLK: " << input.getVCD().clk << "\n";
        std::cout << "\t\t\tRST: " << input.getVCD().rst << "\n";
        std::cout << "\t\t\tSCOPE: " << input.getVCD().scope << "\n";
        for (auto path : input.getVCDPaths()) {
          std::cout << "\t\t\t" << path << std::endl;
        }
      }
      if (input.csvExists() && input.selected_type.count("csv")) {
        std::cout << "CSV"
                  << "\n";
        for (auto path : input.getCSVPaths()) {
          std::cout << "\t\t\t" << path << std::endl;
        }
      }
      if (input.verilogExists() &&
          input.selected_type.count("verilog")) {
        std::cout << "VERILOG"
                  << "\n";
        for (auto path : input.getVerilogPaths()) {
          std::cout << "\t\t\t" << path << std::endl;
        }
      }

      for (auto config : usecase.configs) {
        std::cout << "CONFIG: " << config.type << " " << config.path
                  << std::endl;
      }
      std::cout << "INPUT ADAPTOR: " << usecase.input_adaptor_path
                << std::endl;
      std::cout << "OUTPUT ADAPTOR: " << usecase.output_adaptor_path
                << std::endl;
      std::cout
          << "---------------------------------------------------"
          << "\n";
    }

    //check that all usecases in a test use the same input id
    for (auto test : tests) {
      auto first_usecase = test.use_cases[0];
      for (auto usecase : test.use_cases) {
        messageErrorIf(usecase.input.id != first_usecase.input.id,
                       "All usecases in a test must use the same "
                       "input. In test '" +
                           test.name +
                           "', "
                           "usecase '" +
                           usecase.usecase_id + "' uses input id '" +
                           usecase.input.id + "' while usecase '" +
                           first_usecase.usecase_id +
                           "' uses input id '" +
                           first_usecase.input.id + "'");
      }
    }

  } // for tests

  return tests;
}
} // namespace usmt
