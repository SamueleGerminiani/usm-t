#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Assertion.hh"
#include "Automaton.hh"
#include "AutomataBasedEvaluator.hh"
#include "CSVtraceReader.hh"
#include "EdgeProposition.hh"
#include "Trace.hh"
#include "VCDtraceReader.hh"
#include "assertion_utils.hh"
#include "commandLineParser.hh"
#include "exp.hh"
#include "globals.hh"
#include "message.hh"
#include "minerUtils.hh"
#include "misc.hh"

// Checks that every golden specification holds on the golden trace and
// that it is non-vacuously triggered, i.e. its antecedent is activated
// at least once. Exits with an error otherwise.

using namespace harm;
using namespace usmt;
using namespace expression;

namespace clc {
std::string specFile;
std::string vcdFile;
std::string vcdScope;
std::string clk;
std::string csvFile;
std::string reportFile;
} // namespace clc

static void parseCommandLineArguments(int argc, char *args[]);

//descend through the unary operators (e.g. G) until the implication is found
static TemporalExpressionPtr
findImplication(const TemporalExpressionPtr &formula) {
  TemporalExpressionPtr current = formula;
  while (current != nullptr && !isPropertyImplication(current)) {
    if (current->getItems().size() != 1) {
      return nullptr;
    }
    current = current->getItems()[0];
  }
  return current;
}

//true if the automaton reaches a strong rejecting state when started at
//instant 'start'
static bool rejectsFrom(Automaton *aut, size_t start, size_t end) {
  Automaton::Node *cn = aut->_root;
  while (start < end) {
    for (const auto &edge : cn->_outEdges) {
      if (edge->_prop->evaluate(start)) {
        if (edge->_toNode->_type ==
            Automaton::Node::Type::StrongRejecting) {
          return true;
        } else if (edge->_toNode->_type ==
                   Automaton::Node::Type::StrongAccepting) {
          return false;
        }
        cn = edge->_toNode;
        break;
      }
    }
    start++;
  }
  //the match of the antecedent is incomplete at the end of the trace
  return false;
}

//number of instants at which the antecedent of the implication matches:
//'{ant} |-> false' is rejected exactly when the antecedent matches
static size_t countActivations(const TemporalExpressionPtr &implication,
                               const TracePtr &trace) {
  const TemporalExpressionPtr &ant = getAntecedent(implication);
  std::string antStr =
      "{" + temp2String(ant, Language::SpotLTL, PrintMode::Hide) +
      "} |-> 0";
  Automaton *aut =
      generateAutomatonFromString(antStr, extractPlaceholders(ant));

  size_t activations = 0;
  size_t prevCut = 0;
  for (size_t cut : trace->getCuts()) {
    for (size_t t = prevCut; t <= cut; t++) {
      if (rejectsFrom(aut, t, cut + 1)) {
        activations++;
      }
    }
    prevCut = cut + 1;
  }
  delete aut;
  return activations;
}

static TracePtr readGoldenTrace() {
  TraceReader *tr = nullptr;
  if (clc::vcdFile != "") {
    messageErrorIf(!std::filesystem::exists(clc::vcdFile),
                   "Can not find '" + clc::vcdFile + "'");
    VCDTraceReaderConfig vcd_config;
    vcd_config._clk = clc::clk;
    vcd_config._selectedScope = clc::vcdScope;
    vcd_config._vcdRecursive = 0;
    tr = new VCDtraceReader(clc::vcdFile, vcd_config);
  } else {
    messageErrorIf(!std::filesystem::exists(clc::csvFile),
                   "Can not find '" + clc::csvFile + "'");
    tr = new CSVtraceReader(clc::csvFile);
  }
  TracePtr trace = tr->readTrace();
  delete tr;
  return trace;
}

int main(int arg, char *argv[]) {
  parseCommandLineArguments(arg, argv);

  TracePtr trace = readGoldenTrace();

  messageErrorIf(!std::filesystem::exists(clc::specFile),
                 "Could not find specifications file '" + clc::specFile +
                     "'");
  std::vector<std::string> specStrs;
  std::ifstream specStream(clc::specFile);
  std::string line = "";
  while (std::getline(specStream, line)) {
    if (line.find_first_not_of(" \t\r") != std::string::npos) {
      specStrs.push_back(line);
    }
  }
  messageErrorIf(specStrs.empty(),
                 "No specifications found in '" + clc::specFile + "'");

  std::ofstream report;
  if (clc::reportFile != "") {
    report.open(clc::reportFile);
    messageErrorIf(!report.is_open(),
                   "Can not open '" + clc::reportFile + "'");
    report << "specification;holds;activations\n";
  }

  size_t nFailing = 0;
  size_t nVacuous = 0;
  for (const std::string &specStr : specStrs) {
    AssertionPtr spec = makeAssertion(specStr, trace);
    messageErrorIf(spec == nullptr,
                   "Golden specification '" + specStr +
                       "' cannot be parsed");

    TemporalExpressionPtr implication = findImplication(spec->_formula);
    messageErrorIf(implication == nullptr,
                   "Can not identify the antecedent of the golden "
                   "specification '" +
                       specStr + "'");

    spec->enableEvaluation(trace);
    bool holds = spec->holdsOnTrace();
    size_t activations = countActivations(implication, trace);

    if (!holds) {
      nFailing++;
      messageWarning("Golden specification '" + specStr +
                     "' does not hold on the golden trace");
    }
    if (activations == 0) {
      nVacuous++;
      messageWarning("Golden specification '" + specStr +
                     "' is never activated on the golden trace");
    }

    std::cout << specStr << ": holds " << (holds ? "yes" : "no")
              << ", activations " << activations << "\n";
    if (report.is_open()) {
      report << specStr << ";" << (holds ? "yes" : "no") << ";"
             << activations << "\n";
    }
  }

  std::cout << "Checked " << specStrs.size()
            << " golden specifications on a trace of "
            << trace->getLength() << " instants: " << nFailing
            << " failing, " << nVacuous << " never activated\n";

  if (nFailing > 0 || nVacuous > 0) {
    return 1;
  }
  return 0;
}

void parseCommandLineArguments(int argc, char *args[]) {
  //parse the cmd using an external library
  auto result = parseGoldenCheck(argc, args);

  clc::specFile = result["spec-file"].as<std::string>();
  if (result.count("vcd")) {
    clc::vcdFile = result["vcd"].as<std::string>();
    clc::clk = result["clk"].as<std::string>();
  }
  if (result.count("vcd-ss")) {
    clc::vcdScope = result["vcd-ss"].as<std::string>();
  }
  if (result.count("csv")) {
    clc::csvFile = result["csv"].as<std::string>();
  }
  if (result.count("dump-to")) {
    clc::reportFile = result["dump-to"].as<std::string>();
  }
  if (result.count("ltlf")) {
    clc::useFiniteSemantics = true;
  }

  if (result.count("silent")) {
    clc::silent = true;
    // disable cout
    std::cout.rdbuf(NULL);
  }
  if (result.count("isilent")) {
    clc::isilent = true;
  }
  if (result.count("wsilent")) {
    clc::wsilent = true;
  }
  if (result.count("psilent")) {
    clc::psilent = true;
  }
}
