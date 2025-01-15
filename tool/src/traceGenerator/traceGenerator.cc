#include "EdgeProposition.hh"
#include "exp.hh"
#include "expUtils/expUtils.hh"
#include "expUtils/VarType.hh"
#include "globals.hh"
#include "message.hh"
#include "propositionParsingUtils.hh"
#include "templateParser.hh"
#include "varDeclarationParser.hh"
#include "Template.hh"
#include "z3TestCaseGenerator.hh"
#include <fstream>
#include <spot/tl/parse.hh>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace harm;
using namespace expression;
using Path = std::vector<Automaton::Edge *>;

void parseCommandLineArguments(int argc, char *args[]);
void getTokens(const spot::formula &f, std::vector<std::string> &tokens);
static std::vector<z3::TestCase>
genTraceForPath(const Path &path, size_t pathId, bool free = false);
std::vector<z3::TestCase> genTrace(const std::vector<Path> &paths,
                                   size_t length, bool cut);

std::vector<Path> getPaths(Automaton::Node *node) {

  //  debug
  //  std::cout << "id: " << node->_id << "\n";

  std::vector<Path> paths;
  Automaton::Edge *selfLoop = nullptr;
  for (auto &inEdge : node->_inEdges) {
    if (inEdge->_fromNode->_id == node->_id) {
      if (inEdge->_prop->toString() != "true") {
        selfLoop = inEdge;
      }
      continue;
    }
    std::vector<Path> ret = getPaths(inEdge->_fromNode);
    for (auto &path : ret) {
      path.push_back(inEdge);
      paths.push_back(path);
    }
  }

  if (node->_inEdges.size() - (selfLoop != nullptr ? 1 : 0) == 0) {
    // base case
    paths.emplace_back();
  }

  if (selfLoop != nullptr) {
    std::vector<Path> selfLoopPaths;
    for (auto &path : paths) {
      auto copy = path;
      copy.push_back(selfLoop);
      selfLoopPaths.push_back(copy);
    }
    paths.insert(paths.end(), selfLoopPaths.begin(), selfLoopPaths.end());
  }
  return paths;
}

Proposition *edgePropToProp(EdgeProposition *ep) {
  // And
  if (dynamic_cast<EdgeAnd *>(ep) != nullptr) {
    PropositionAnd *ret = new PropositionAnd();
    for (size_t i = 0; i < ep->getOperands().size(); i++) {
      ret->addItem(edgePropToProp(ep->getOperands()[i]));
    }
    return ret;
  }

  // Or
  if (dynamic_cast<EdgeOr *>(ep) != nullptr) {
    PropositionOr *ret = new PropositionOr();
    for (size_t i = 0; i < ep->getOperands().size(); i++) {
      ret->addItem(edgePropToProp(ep->getOperands()[i]));
    }
    return ret;
  }

  // Not
  if (dynamic_cast<EdgeNot *>(ep) != nullptr) {
    return makeExpression<PropositionNot>(edgePropToProp(ep->getOperands()[0]));
  }

  // Atomic proposition
  if (dynamic_cast<EdgePlaceholder *>(ep) != nullptr) {
    return *dynamic_cast<EdgePlaceholder *>(ep)->_toProp;
  }

  // Constants
  if (dynamic_cast<EdgeTrue *>(ep) != nullptr) {
    return new BooleanConstant(true, VarType::Bool, 1, 0);
  }

  if (dynamic_cast<EdgeNot *>(ep) != nullptr) {
    return new BooleanConstant(false, VarType::Bool, 1, 0);
  }

  messageError("Error in edgePropToProp");
  return nullptr;
}


// WARNING: this function has an internal state!
static std::vector<z3::TestCase> genTraceForPath(const Path &path,
                                                 size_t pathId, bool free) {
  static std::map<std::pair<size_t, size_t>, bool> isExhausted;
  static std::map<std::pair<size_t, size_t>, std::vector<z3::TestCase>>
      alreadyGen;

  //debug
  //static std::map<std::pair<size_t, size_t>, size_t> ntests;

  if (free) {
    isExhausted.clear();
    alreadyGen.clear();
    //debug
   // ntests.clear();
    return std::vector<z3::TestCase>();
  }

  z3::Z3TestCaseGenerator tcg;
  std::vector<z3::TestCase> subTrace;

  for (size_t j = 0; j < path.size(); j++) {
    auto &edge = path[j];

    // avoid z3 errors
    if (edge->_prop->toString() == "!(true)" ||
        edge->_prop->toString() == "false")
      continue;

    if (isExhausted.count(std::make_pair(pathId, j)) == 0) {
      isExhausted[std::make_pair(pathId, j)] = false;
    }
    if (!isExhausted.at(std::make_pair(pathId, j)) &&
        alreadyGen[std::make_pair(pathId, j)].size() <
            clc::maxCore) {
      auto tcs = tcg.generateTestCase(*edgePropToProp(edge->_prop), 1,
                                      alreadyGen[std::make_pair(pathId, j)]);
      messageErrorIf(tcs.empty() &&
                         alreadyGen.at(std::make_pair(pathId, j)).empty(),
                     "Error in z3");
      if (!tcs.empty()) {
        subTrace.insert(subTrace.end(), tcs.begin(), tcs.end());
        auto &list = alreadyGen.at(std::make_pair(pathId, j));
        //        debug
        //        ntests[std::make_pair(pathId, j)]++;
        list.insert(list.end(), tcs.begin(), tcs.end());
      } else {
        // first time exhausted --> executed only once
        isExhausted.at(std::make_pair(pathId, j)) = true;
        auto &prev = alreadyGen.at(std::make_pair(pathId, j));
        subTrace.push_back(prev.back());
      }
    } else {
      auto &prev = alreadyGen.at(std::make_pair(pathId, j));

      //      debug
      //      std::cout << edge->_prop->toString() << ": ";
      //      std::cout << ntests.at(std::make_pair(pathId, j)) << "\n";
      size_t randNum = rand() % ((prev.size() - 1) - 0 + 1) + 0;
      subTrace.push_back(prev[randNum]);
    }
  }
  return subTrace;
}

std::vector<z3::TestCase> genTrace(const std::vector<Path> &paths,
                                   size_t length, bool cut) {
  if (paths.empty()) {
    return std::vector<z3::TestCase>();
  }

  size_t traceLength = 0;
  std::vector<z3::TestCase> trace;

  // free function state
  genTraceForPath(paths[0], 0, true);

  // generate the shortest trace
  for (size_t i = 0; i < paths.size(); i++) {
    auto subTrace = genTraceForPath(paths[i], i);
    trace.insert(trace.end(), subTrace.begin(), subTrace.end());
    if (cut) {
      trace.push_back(z3::TestCase({std::make_pair("@cut", z3::TypeValue())}));
    }
    traceLength += subTrace.size();
  }

  // extend the trace
  while (1) {
    for (size_t i = 0; i < paths.size(); i++) {
      if (traceLength >= length) {
        goto finish;
      }
      auto subTrace = genTraceForPath(paths[i], i);
      if (subTrace.empty()) {
        goto finish;
      }
      traceLength += subTrace.size();
      trace.insert(trace.end(), subTrace.begin(), subTrace.end());
      if (cut) {
        trace.push_back(
            z3::TestCase({std::make_pair("@cut", z3::TypeValue())}));
      }
    }
  }
finish:;

  return trace;
}

void parseCommandLineArguments(int argc, char *args[]) {
  auto result = parseTraceGenerator(argc, args);

  if (result.count("f")) {
    clc::formula = result["f"].as<std::string>();
    std::cout << "Formula: " << clc::formula << "\n";
  }
  if (result.count("v")) {
    clc::varsDecl = result["v"].as<std::string>();
    std::cout << "Variables: " << clc::varsDecl << "\n";
  }
  if (result.count("l")) {
    clc::length = result["l"].as<size_t>();
    std::cout << "Trace length: " << clc::length << "\n";
  }
  if (result.count("o")) {
    clc::outFile = result["o"].as<std::string>();
    std::cout << "Outfile: " << clc::outFile << "\n";
  }
  if (result.count("core")) {
    clc::maxCore = result["core"].as<size_t>();
    std::cout << "Core length: " << clc::maxCore << "\n";
  }
  if (result.count("dto")) {
    clc::dtOperator = result["dto"].as<std::string>();
    std::cout << "DTO: " << clc::dtOperator << "\n";
  }
}

int main(int arg, char *argv[]) {
  parseCommandLineArguments(arg, argv);

  // read variables
  std::vector<DataType> vars_dt;
  std::map<std::string, std::string> varToType;
  auto rows = csv::parse(clc::varsDecl);
  for (auto &r : rows.get_col_names()) {
    auto var = parseVariable(r);
    vars_dt.push_back(toDataType(var.first, var.second.first,
                                                 var.second.second));
    varToType[var.first] = var.second.first;
  }

  // allocate trace
  Trace *trace = new Trace(vars_dt, clc::length);

  std::vector<std::tuple<Proposition *, size_t, size_t>> dtOperands;
  Template *ass = parseAssertion(trace, dtOperands);

  // debug
  std::cout << ass->getColoredAssertion() << "\n";

  std::vector<z3::TestCase> z3trace;

  // get main trace
  Automaton *autOn = ass->buildDiamondAutomaton();
  auto paths = getPaths(autOn->_accepting);
  std::cout << "N Paths: " << paths.size() << "\n";
  for (auto p : paths) {
    for (auto e : p) {
      std::cout << e->_prop->toString() << " --> ";
    }
    std::cout << "\n";
  }

  z3trace = genTrace(paths, clc::length / 2,
                     dynamic_cast<DTNCReps *>(ass->getDT()) != nullptr);

  if (ass->getDT() != nullptr && ass->getDT()->getNChoices() > 1) {
    // get the counterexample trace for dt mining
    auto ce = getCounterExamples(
        ass, dtOperands, dynamic_cast<DTNCReps *>(ass->getDT()) != nullptr);
    // add ce to the main trace->t
    z3trace.insert(z3trace.end(), ce.begin(), ce.end());
  }

  if (dynamic_cast<DTNCReps *>(ass->getDT()) == nullptr) {
    z3trace.push_back(z3::TestCase({std::make_pair("@cut", z3::TypeValue())}));
  }

  //  debug
  //  for (auto i : z3trace) {
  //      std::cout << i << "\n";
  //  }
  // write to file
  size_t fileID = 0;
  std::vector<z3::TestCase> ft;
  for (auto &e : z3trace) {
    if (e.size() == 1 && e[0].first == "@cut") {
      if (!ft.empty()) {
        dumpTrace(ft, varToType, fileID++);
      }
      ft.clear();
    } else {
      ft.push_back(e);
    }
  }

  delete autOn;
  delete trace;
  delete ass;
  return 0;
}
