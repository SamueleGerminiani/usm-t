#include "Automaton.hh"
#include "Evaluator.hh"
#include "Location.hh"
#include "Semaphore.hh"
#include "Trace.hh"
#include "Trinary.hh"
#include "formula/temporal/TemporalExpression.hh"
#include "spot/twa/twagraph.hh"

namespace harm {
class PlaceholderPack;

///@brief This class is used to evaluate a formula using an automaton
class AutomataBasedEvaluator : public Evaluator {

public:
  AutomataBasedEvaluator(const TemporalExpressionPtr &formula,
                         const harm::TracePtr &trace);
  ~AutomataBasedEvaluator();

  Automaton *getAutomaton() { return _automaton; }

  virtual bool holdsOnTrace() override;

private:
  bool evaluateAutomaton(size_t start, size_t end);

  void generateAutomaton();

  void changeTrace(const harm::TracePtr &trace) override;

  Automaton *_automaton = nullptr;
};
// shared pointer to the AutomataBasedEvaluator
using AutomataBasedEvaluatorPtr =
    std::shared_ptr<AutomataBasedEvaluator>;

std::shared_ptr<spot::twa_graph>
generateDeterministicSpotAutomaton(const spot::formula &formula);

/** \brief builds a custom automaton (see class Automaton) from a spotLTL automaton
   */
Automaton *buildAutomatonFromSpot(spot::twa_graph_ptr &automata,
                                  const PlaceholderPack &pack);
Automaton *
generateAutomatonFromTemporal(const TemporalExpressionPtr &formula);

Automaton *
generateAutomatonFromString(const std::string &spotFormulaStr,
                            const PlaceholderPack &ppack);
} // namespace harm
