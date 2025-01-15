#include "gedlibWrapper.hh"
#include "Assertion.hh"
#include "Automaton.hh"
#include "EdgeProposition.hh"
#include "boolean_automaton_cost.hpp"
#include "src/env/ged_env.hpp"

std::map<std::pair<std::string, std::string>, double>
    edge_rel_cost_map;

namespace usmt {
using namespace harm;

double computeEditDistanceSimilarity(const SerializedAutomaton &sa1,
                                     const SerializedAutomaton &sa2) {

  std::unordered_map<std::string, EdgeProposition *>
      edgeStrToProposition;

  // Instantiate GED environment
  ged::GEDEnv<int, double, std::string> env;

  ged::GEDGraph::GraphID graph1 = env.add_graph("graph1", "ClassA");
  for (auto &[id, label] : sa1.nodes) {
    env.add_node(graph1, id, label);
  }
  for (auto &[from, to, edgeProp] : sa1.edges) {
    std::string edgeStr = edgeProp->toString();
    edgeStrToProposition[edgeStr] = edgeProp;
    env.add_edge(graph1, from, to, edgeStr);
  }

  ged::GEDGraph::GraphID graph2 = env.add_graph("graph1", "ClassA");

  for (auto &[id, label] : sa2.nodes) {
    env.add_node(graph2, id, label);
  }
  for (auto &[from, to, edgeProp] : sa2.edges) {
    std::string edgeStr = edgeProp->toString();
    edgeStrToProposition[edgeStr] = edgeProp;
    env.add_edge(graph2, from, to, edgeStr);
  }

  env.set_edit_costs(new ged::BooleanAutomaton<double, std::string>(
      edgeStrToProposition));

  env.init(ged::Options::InitType::EAGER_WITHOUT_SHUFFLED_COPIES);

  // Set the GED computation method
  env.set_method(ged::Options::GEDMethod::STAR,
                 "--optimal TRUE --threads 1");

  // Initialize the selected method
  env.init_method();

  // Run GED computation between the two graphs
  env.run_method(graph1, graph2, 0);

  // Retrieve and print the results
  double upper_bound = env.get_upper_bound(graph1, graph2);

  double similarity =
      1.f - (upper_bound /
             (double)std::max(
                 sa1.getNumberEdges() - 2 + sa1.getNumberNodes(),
                 sa2.getNumberEdges() - 2 + sa2.getNumberNodes()));
  return similarity;
}

SerializedAutomaton serializeAutomaton(Automaton *aut) {
  SerializedAutomaton sa;
  for (const auto &[id, node] : aut->_idToNode) {
    int type = 0;
    if (node->_type == harm::Automaton::Node::Type::Accepting) {
      type = 1;
    } else if (node->_type ==
               harm::Automaton::Node::Type::Rejecting) {
      type = -1;
    } else if (node->_type == harm::Automaton::Node::Type::Pending) {
      type = 0;
    } else {
      messageError("Unknown node type");
    }

    sa.nodes.push_back({(int)id, type});

    for (const auto &edge : node->_outEdges) {
      int from = (int)edge->_fromNode->_id;
      int to = (int)edge->_toNode->_id;
      if (from == to &&
          dynamic_cast<EdgeTrue *>(edge->_prop) != nullptr) {
        continue;
      }
      sa.edges.push_back({from, to, edge->_prop});
    }
  }
  return sa;
}
} // namespace usmt
