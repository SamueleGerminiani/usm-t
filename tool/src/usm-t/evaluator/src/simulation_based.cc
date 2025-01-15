
#include "simulation_based.hh"
#include "AutomataBasedEvaluator.hh"
#include "EdgeProposition.hh"
#include "exp.hh"
#include "expUtils/expUtils.hh"
#include "globals.hh"
#include "message.hh"
#include "propositionParsingUtils.hh"
#include "varDeclarationParser.hh"
#include "z3TestCaseGenerator.hh"
#include <fstream>
#include <spot/tl/parse.hh>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace harm;
using namespace expression;
namespace usmt {
static std::vector<Path>
getPaths(harm::Automaton::Node *node, size_t depth, size_t maxDepth,
         size_t maxLoops,
         std::unordered_map<Automaton::Node *, size_t> times_visited);

std::vector<Path> getPaths(Automaton::Node *node, size_t maxDepth,
                           size_t maxLoops) {
  return getPaths(node, 0, maxDepth, maxLoops, {});
}

std::vector<Path> getPaths(
    Automaton::Node *node, size_t depth, size_t maxDepth,
    size_t maxLoops,
    std::unordered_map<Automaton::Node *, size_t> times_visited) {

  if (depth == 0) {
    times_visited.clear();
  } else {
    times_visited[node]++;
  }

  //  debug
  //std::cout << "id: " << node->_id << "\n";
  if (depth >= maxDepth) {
    return {{}};
  }

  if (times_visited.count(node) &&
      times_visited.at(node) > maxLoops) {
    return {{}};
  } else {
    times_visited[node]++;
  }

  std::vector<Path> paths;

  for (auto &outEdge : node->_outEdges) {
    //std::cout << inEdge->_prop->toString() << "\n";
    depth++;
    std::vector<Path> ret = getPaths(
        outEdge->_toNode, depth, maxDepth, maxLoops, times_visited);
    depth--;
    for (auto &path : ret) {
      path.insert(path.begin(), outEdge);
      paths.push_back(path);
    }
  }

  return paths;
}

//std::vector<Path> getPaths(Automaton::Node *node, size_t depth,
//                           size_t maxDepth) {
//
//  //  debug
//  //std::cout << "id: " << node->_id << "\n";
//  if (depth >= maxDepth) {
//    return {{}};
//  }
//
//  std::vector<Path> paths;
//
//  for (auto &inEdge : node->_inEdges) {
//    //std::cout << inEdge->_prop->toString() << "\n";
//    depth++;
//    std::vector<Path> ret =
//        getPaths(inEdge->_fromNode, depth, maxDepth);
//    depth--;
//    for (auto &path : ret) {
//      path.push_back(inEdge);
//      paths.push_back(path);
//    }
//  }
//
//  return paths;
//}

void evaluateSimulationBased() {}
} // namespace usmt
