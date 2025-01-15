#include "Automaton.hh"
#include <vector>
using Path = std::vector<harm::Automaton::Edge *>;

namespace usmt {
std::vector<Path> getPaths(harm::Automaton::Node *node,
                           size_t maxDepth, size_t maxLoops = -1);
}
