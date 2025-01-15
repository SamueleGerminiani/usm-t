#include "Assertion.hh"
#include "Automaton.hh"
#include "EdgeProposition.hh"
#include <map>

namespace usmt {
using namespace harm;

struct SerializedAutomaton;
SerializedAutomaton serializeAutomaton(Automaton *aut);
class SerializedAutomaton {

public:
  SerializedAutomaton() = default;
  //edges <source, destination, edge_label>
  std::vector<std::tuple<int, int, EdgeProposition *>> edges;
  //nodes <id, type>
  std::vector<std::tuple<int, int>> nodes;

  size_t getSize() const { return edges.size() + nodes.size(); }
  size_t getNumberEdges() const { return edges.size(); }
  size_t getNumberNodes() const { return nodes.size(); }
};

double computeEditDistanceSimilarity(const SerializedAutomaton &sa1,
                                     const SerializedAutomaton &sa2);

SerializedAutomaton serializeAutomaton(Automaton *aut);
} // namespace usmt
