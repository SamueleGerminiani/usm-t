#include "test_reader.hh"
#include "Test.hh"
#include "message.hh"
#include "xmlUtils.hh"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace usmt {
using namespace rapidxml;

std::vector<Test> parseTests(XmlNode *root);

std::vector<Test> readTestFile(const std::string &filename) {

  try {
    rapidxml::file<> xmlFile(filename.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    XmlNode *root = doc.first_node("usm-t");
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

void parseInput(XmlNode *inputNode, Input &input) {
  input.type = getAttributeValue(inputNode, "type", "");
  input.path = getAttributeValue(inputNode, "path", "");
  messageErrorIf(input.type.empty() || input.path.empty(),
                 "Input type or path cannot be empty in 'input' tag");
  input.clk = getAttributeValue(inputNode, "clk", "");
  input.rst = getAttributeValue(inputNode, "rst", "");
  if (input.type == "vcd") {
    messageErrorIf(input.clk.empty(),
                   "VCD input '" + input.path +
                       "' must have a clk in the xml tag");
  }

  input.scope = getAttributeValue(inputNode, "scope", "");
}

void parseConfigs(XmlNode *configNode, std::vector<Config> &configs) {
  XmlNodeList configNodes;
  getNodesFromName(configNode, "config", configNodes);
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
                   comp.with_strategy == "edit_distance" ||
                   comp.with_strategy == "syntactic_similarity" ||
                   comp.with_strategy == "time_to_mine"),
                 "Comparator strategy '" + comp.with_strategy +
                     "' not supported, supported strategies are "
                     "'fault_coverage', 'semantic_equivalence', "
                     "'edit_distance', 'syntactic_similarity' and "
                     "'time_to_mine'");
  comp.expected = getAttributeValue(compareNode, "expected", "");
  messageErrorIf(comp.expected.empty() &&
                     (comp.with_strategy == "semantic_equivalence" ||
                      (comp.with_strategy == "systactic_similarity" ||
                       comp.with_strategy == "edit_distance")),
                 "Must specify a path to a set of golden assertions "
                 "with the attribute 'expected' when using the "
                 "'semantic_equivalence', 'edit_distance' or "
                 "'syntactic_similarity strategy");

  comp.faulty_traces =
      getAttributeValue(compareNode, "faulty_traces", "");
  comp.trace_type = getAttributeValue(compareNode, "trace_type", "");
  if (comp.trace_type == "vcd") {
    comp.clk = getAttributeValue(compareNode, "clk", "");
    messageErrorIf(comp.clk == "",
                   "VCD trace type must have a clk in the xml tag of "
                   "a fault coverage comparator");
    comp.scope = getAttributeValue(compareNode, "scope", "");
    messageErrorIf(comp.scope == "",
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

UseCase parseUseCase(XmlNode *usecaseNode) {
  // Parse usecase
  UseCase usecase;
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
  std::vector<rapidxml::xml_node<> *> inputNodes;
  getNodesFromName(usecaseNode, "input", inputNodes);
  for (auto n : inputNodes) {
    Input new_input;
    parseInput(n, new_input);
    usecase.input.push_back(new_input);
  }

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
                 "Input adaptor path cannot be empty");

  // Parse output adaptor
  std::vector<rapidxml::xml_node<> *> output_adaptorNodes;
  getNodesFromName(usecaseNode, "output_adaptor",
                   output_adaptorNodes);
  usecase.output_adaptor_path =
      getAttributeValue(output_adaptorNodes[0], "path", "");
  messageErrorIf(usecase.output_adaptor_path.empty(),
                 "Output adaptor path cannot be empty");

  return usecase;
}

std::vector<Test> parseTests(XmlNode *root) {

  //parse usecases declarations
  XmlNodeList usecaseNodes;
  getNodesFromName(root, "usecase", usecaseNodes);
  std::unordered_map<std::string, UseCase> idToUseCase;
  for (auto usecaseNode : usecaseNodes) {
    auto usecase = parseUseCase(usecaseNode);
    idToUseCase[usecase.usecase_id] = usecase;
  }

  //parse tests
  std::vector<Test> tests;
  XmlNodeList testNodes;
  getNodesFromName(root, "test", testNodes);
  messageErrorIf(testNodes.size() == 0,
                 "There should be at least one test tag");

  for (auto testNode : testNodes) {
    Test test;
    test.name = getAttributeValue(testNode, "name", "");
    //test.mode = getAttributeValue(testNode, "mode", "");
    //messageErrorIf(test.name.empty() || test.mode.empty(),
    //               "Test name or mode cannot be empty in test tag");
    messageErrorIf(test.name.empty(),
                   "Test name cannot be empty in test tag");

    //parse comparators
    XmlNodeList compareNodes;
    getNodesFromName(testNode, "compare", compareNodes);
    for (auto compareNode : compareNodes) {
      test.comparators.push_back(parseCompare(compareNode));
    }
    messageWarningIf(test.comparators.empty(),
                     "Non comparators found in test '" + test.name +
                         "'");

    //Parse usecases used in the test
    XmlNodeList usecaseNodes;
    getNodesFromName(testNode, "usecase", usecaseNodes);
    for (auto usecaseNode : usecaseNodes) {
      std::string id = getAttributeValue(usecaseNode, "id", "");
      messageErrorIf(id.empty(),
                     "Usecase id cannot be empty in test tag");
      if (idToUseCase.count(id) == 0) {
        messageWarning("Usecase with id '" + id + "' not found");
      } else {
        test.use_cases.push_back(idToUseCase[id]);
      }
    }

    tests.push_back(test);
  }

  //print tests
  for (auto test : tests) {
    std::cout << "Test: " << test.name << " Mode: " << test.mode
              << std::endl;
    std::cout << "\tComparators:"
              << "\n";
    for (auto comp : test.comparators) {
      std::cout << "\t\t\twith_strategy: " << comp.with_strategy
                << "\n";
      std::cout << "\t\t\texpected: " << comp.expected << "\n";
    }

    for (auto usecase : test.use_cases) {
      std::cout << "Usecase: " << usecase.usecase_id << "\n";
      std::cout << "\t\t\t Miner: " << usecase.miner_name
                << std::endl;
      for (auto input : usecase.input) {
        std::cout << "\t\t\t Input: " << input.type << " "
                  << input.path << " " << input.clk << std::endl;
      }
      for (auto config : usecase.configs) {
        std::cout << "\t\t\t Config: " << config.type << " "
                  << config.path << std::endl;
      }
      std::cout << "\t\t\t Input Adaptor: "
                << usecase.input_adaptor_path << std::endl;
      std::cout << "\t\t\t Output Adaptor: "
                << usecase.output_adaptor_path << std::endl;
    }
  }

  return tests;
}
} // namespace usmt
