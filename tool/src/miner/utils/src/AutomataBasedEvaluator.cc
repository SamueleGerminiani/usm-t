#include "AutomataBasedEvaluator.hh"
#include "Automaton.hh"
#include "EdgeProposition.hh"
#include "Semaphore.hh"
#include "globals.hh"
#include "minerUtils.hh"
#include "propositionParsingUtils.hh"
#include "spot/misc/trival.hh"
#include <stdexcept>
#include <thread>
#include <unordered_map>

#ifdef SPOTLTL
#include "spot/twa/bddprint.hh"
#include "spot/twa/twa.hh"
#include "spot/twa/twagraph.hh"
#include "spot/twaalgos/translate.hh"
#include <spot/tl/formula.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/postproc.hh>
#endif

namespace harm {

//static functions

#ifdef SPOTLTL

/** \brief converts a spot LTL formula to an edge custom proposition
   */
static EdgeProposition *
spotEdgeToProposition(const spot::formula &f,
                      const PlaceholderPack &pack);
/// @brief syntax sugar for printing a spot formula
static std::string to_string(const spot::formula &f);

///@brief returns tree if the string is a tokenplaceholder
static bool isPlaceholderStr(const std::string &s);
///@brief returns true if the string is an instance
static bool isInstStr(const std::string &s);
///@brief returns true if the string is a DTO placeholder
static bool isDTO_Str(const std::string &s);

#endif

//------------------------------AutomatataBased class-----------------------------------
//
AutomataBasedEvaluator::AutomataBasedEvaluator(
    const TemporalExpressionPtr &formula, const TracePtr &trace)
    : Evaluator(formula, trace) {
  generateAutomaton();
}

AutomataBasedEvaluator::~AutomataBasedEvaluator() {
  delete _automaton;
}

bool AutomataBasedEvaluator::holdsOnTrace() {
  auto &cuts = _trace->getCuts();
  size_t prevCut = 0;
  for (size_t i = 0; i < cuts.size(); i++) {
    auto &cut = cuts[i];
    if (!evaluateAutomaton(prevCut, cut + 1)) {
      return false;
    }

    prevCut = cut + 1;
  }

  return true;
}

bool AutomataBasedEvaluator::evaluateAutomaton(size_t start,
                                               size_t end) {
  Automaton::Node *cn = _automaton->_root;
  /* visit the automaton by evaluating the edges (which are propositions)
   */
  while (start < end) {
    for (const auto &edge : cn->_outEdges) {
      // if "the current cn->_outEdges[i] is true at instant 'start'"
      if (edge->_prop->evaluate(start)) {
        if (edge->_toNode->_type ==
            Automaton::Node::Type::Rejecting) {
          return false;
        }

        // go to the next state
        cn = edge->_toNode;
        break;
      }
    }
    // each start we change state, start increases by 1
    start++;
  }

  return true;
}

void AutomataBasedEvaluator::generateAutomaton() {

#ifdef SPOTLTL
  {
    static std::mutex spotGuard;
    std::lock_guard<std::mutex> lock{spotGuard};
    //retrieve the string representation of temporal formula
    std::string spotFormulaStr =
        temp2String(_formula, Language::SpotLTL, PrintMode::Hide);
    //retrieve the spot formula
    spot::formula spotFormula =
        spot::parse_infix_psl(spotFormulaStr).f;
    //generate the spot automaton
    std::shared_ptr<spot::twa_graph> spotAutomaton =
        generateDeterministicSpotAutomaton(spotFormula);
    //extract the placeholders
    auto placeholderPack = extractPlaceholders(_formula);
    //build the harm automaton
    _automaton =
        buildAutomatonFromSpot(spotAutomaton, placeholderPack);
    //std::cout << printAutomaton(_automaton) << "\n";
  }
#else
  messageError("No automata generator library provided");
  //other automata-based libraries
#endif
}

Automaton *
generateAutomatonFromTemporal(const TemporalExpressionPtr &formula) {

#ifdef SPOTLTL
  {
    static std::mutex spotGuard;
    std::lock_guard<std::mutex> lock{spotGuard};
    //retrieve the string representation of temporal formula
    std::string spotFormulaStr =
        temp2String(formula, Language::SpotLTL, PrintMode::Hide);
    //retrieve the spot formula
    spot::formula spotFormula =
        spot::parse_infix_psl(spotFormulaStr).f;
    //generate the spot automaton
    std::shared_ptr<spot::twa_graph> spotAutomaton =
        generateDeterministicSpotAutomaton(spotFormula);
    //extract the placeholders
    auto placeholderPack = extractPlaceholders(formula);
    //build the harm automaton
    return buildAutomatonFromSpot(spotAutomaton, placeholderPack);
    //std::cout << printAutomaton(_automaton) << "\n";
  }
#else
  messageError("No automata generator library provided");
  //other automata-based libraries
#endif
}

Automaton *
generateAutomatonFromString(const std::string &spotFormulaStr,
                            const PlaceholderPack &ppack) {

#ifdef SPOTLTL
  {
    static std::mutex spotGuard;
    std::lock_guard<std::mutex> lock{spotGuard};
    //retrieve the string representation of temporal formula
    //retrieve the spot formula
    spot::formula spotFormula =
        spot::parse_infix_psl(spotFormulaStr).f;
    //generate the spot automaton
    std::shared_ptr<spot::twa_graph> spotAutomaton =
        generateDeterministicSpotAutomaton(spotFormula);
    //build the harm automaton
    return buildAutomatonFromSpot(spotAutomaton, ppack);
    //std::cout << printAutomaton(_automaton) << "\n";
  }
#else
  messageError("No automata generator library provided");
  //other automata-based libraries
#endif
}

//--------------------------------------------------------------------------------------

///spot related functions
#ifdef SPOTLTL

std::string to_string(const spot::formula &f) {
  std::stringstream ss;
  ss << f;
  return ss.str();
}

std::shared_ptr<spot::twa_graph>
generateDeterministicSpotAutomaton(const spot::formula &formula) {
  static std::unordered_map<std::string,
                            std::shared_ptr<spot::twa_graph>>
      cache;
  std::string formulaStr = to_string(formula);
  if (cache.count(formulaStr)) {
    return cache.at(formulaStr);
  }

  spot::translator trans;
  trans.set_pref(spot::postprocessor::Deterministic);
  auto aut = trans.run(formula);

  spot::postprocessor post2;
  post2.set_pref(spot::postprocessor::Complete);
  aut = post2.run(aut);

  if (!spot::is_deterministic(aut)) {
    throw std::runtime_error(
        "The formula '" + to_string(formula) +
        "' generates a non-deterministic automaton");
  }

  if (!spot::is_complete(aut)) {
    throw std::runtime_error("The formula '" + to_string(formula) +
                             "' generates an incomplete automaton");
  }

  if (!aut->prop_state_acc().is_true()) {
    throw std::runtime_error(
        "The formula '" + to_string(formula) +
        "' generates an automaton that does not allow "
        "state-based acceptance");
  }

  if (aut->num_states() == 1) {
    throw std::runtime_error(
        "The formula '" + to_string(formula) +
        "' generates an automaton that is trivially true or false");
  }

  //add to cache
  cache.insert({formulaStr, aut});

  return aut;
}

Automaton *buildAutomatonFromSpot(spot::twa_graph_ptr &automata,
                                  const PlaceholderPack &pack) {
  size_t stateCount = 0;
  std::unordered_map<size_t, size_t> hashToId;
  auto initState = automata->get_init_state();
  Automaton *retA = new Automaton();

  // dictionary to extract edge propositions
  auto bddDict = automata->get_dict();
  // keep track of visited states
  std::unordered_map<size_t, const spot::state *> visited;
  // queue
  std::deque<const spot::state *> fringe;

  // visit the automata starting from the initial state(BFS)
  fringe.push_front(initState);
  // mark the first state as visited
  visited.insert({initState->hash(), initState});

  while (!fringe.empty()) {
    const spot::state *currState = fringe.back();
    fringe.pop_back();

    if (hashToId.count(currState->hash()) == 0) {
      //create a link to retrieve the Node (only if it doesn't already exist)
      hashToId[currState->hash()] = stateCount++;
      retA->_idToNode[hashToId.at(currState->hash())] =
          new Automaton::Node(hashToId.at(currState->hash()),
                              Automaton::Node::Type::Pending);
    }

    // identify if the current state is a terminal state
    auto it = automata->succ_iter(currState);
    if (it->first() && (it->dst()->hash() == currState->hash()) &&
        !it->next()) {
      if (automata->state_is_accepting(currState)) {
        // acceptance state: the evaluation returns true
        retA->_idToNode.at(hashToId.at(currState->hash()))->_type =
            Automaton::Node::Type::Accepting;
        retA->_accepting =
            retA->_idToNode.at(hashToId.at(currState->hash()));
      } else {
        // rejecting state: the evaluation returns false
        retA->_idToNode.at(hashToId.at(currState->hash()))->_type =
            Automaton::Node::Type::Rejecting;
        retA->_rejecting =
            retA->_idToNode.at(hashToId.at(currState->hash()));
      }
    }
    delete it;

    // for each out edge
    for (auto s : automata->succ(currState)) {
      if (visited.count(s->dst()->hash()) == 0) {
        // mark as visited
        visited.insert({s->dst()->hash(), s->dst()});
        // put on the queue
        fringe.push_back(s->dst());
      }

      // retrieve the spotLTL representation of an edge
      spot::formula edge = spot::parse_formula(
          spot::bdd_format_formula(bddDict, s->cond()));

      if (hashToId.count(s->dst()->hash()) == 0) {
        //Again: create a link to retrieve the Node (only if it doesn't exist already)
        hashToId[s->dst()->hash()] = stateCount++;
        retA->_idToNode[hashToId.at(s->dst()->hash())] =
            new Automaton::Node(hashToId.at(s->dst()->hash()),
                                Automaton::Node::Type::Pending);
      }
      // add the custom edge to the custom node
      EdgeProposition *newEdgeProp =
          spotEdgeToProposition(edge, pack);
      Automaton::Edge *newEdge = new Automaton::Edge(
          newEdgeProp,
          retA->_idToNode.at(hashToId.at(s->dst()->hash())),
          retA->_idToNode.at(hashToId.at(currState->hash())));

      retA->_idToNode.at(hashToId.at(currState->hash()))
          ->_outEdges.push_back(newEdge);
      retA->_idToNode.at(hashToId.at(s->dst()->hash()))
          ->_inEdges.push_back(newEdge);
    }
  }

  // set the initial state of the automaton
  retA->_root = retA->_idToNode.at(0);

  return retA;
}

EdgeProposition *spotEdgeToProposition(const spot::formula &f,
                                       const PlaceholderPack &pack) {
  //visit the expression depending on the type of operator

  // And
  if (f.is(spot::op::And)) {
    std::vector<EdgeProposition *> operands;
    for (size_t i = 0; i < f.size(); i++) {
      operands.push_back(spotEdgeToProposition(f[i], pack));
    }

    return new EdgeAnd(operands);
  }

  // Or
  if (f.is(spot::op::Or)) {
    std::vector<EdgeProposition *> operands;
    for (size_t i = 0; i < f.size(); i++) {
      operands.push_back(spotEdgeToProposition(f[i], pack));
    }

    return new EdgeOr(operands);
  }

  // Not
  if (f.is(spot::op::Not)) {
    return new EdgeNot(spotEdgeToProposition(f[0], pack));
  }

  // Atomic proposition
  if (f.is(spot::op::ap)) {
    if (isPlaceholderStr(f.ap_name())) {
      messageErrorIf(!pack._tokenToPP.count(f.ap_name()),
                     "Placeholder not found: " + f.ap_name());
      return new EdgePlaceholder(pack._tokenToPP.at(f.ap_name()),
                                 f.ap_name());
    } else if (isInstStr(f.ap_name())) {
      messageErrorIf(!pack._tokenToInst.count(f.ap_name()),
                     "Inst not found: " + f.ap_name());
      return new EdgeInst(pack._tokenToInst.at(f.ap_name()),
                          f.ap_name());
    } else if (isDTO_Str(f.ap_name())) {
      messageErrorIf(!pack._tokenToPP.count(f.ap_name()),
                     "DTO Placeholder not found: " + f.ap_name());
      return new EdgePlaceholder(pack._tokenToPP.at(f.ap_name()),
                                 f.ap_name());
    } else {
      messageError("Unknown atomic proposition: " + f.ap_name());
    }
  }

  // Constants
  if (f.is_tt()) {
    return new EdgeTrue();
  }

  if (f.is_ff()) {
    return new EdgeFalse();
  }

  messageError("Error in spot edge formula");
  return nullptr;
}

bool isPlaceholderStr(const std::string &s) { return s[0] == 'P'; }
bool isInstStr(const std::string &s) {
  return s.size() > 5 && s.substr(0, 5) == "_inst";
}
bool isDTO_Str(const std::string &s) {
  return s.size() > 2 && s.substr(0, 3) == "_dt";
}

#endif

void AutomataBasedEvaluator::changeTrace(
    const harm::TracePtr &trace) {

  _trace = trace;
  expression::changeTrace(_formula, trace);
  //extend the cache if the new trace is longer
}

} // namespace harm
