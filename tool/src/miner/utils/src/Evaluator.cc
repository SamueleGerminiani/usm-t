#include "Evaluator.hh"
#include "AutomataBasedEvaluator.hh"
#include "globals.hh"

namespace harm {

Evaluator::Evaluator(const TemporalExpressionPtr &formula,
                     const TracePtr &trace)
    : _formula(formula), _trace(trace) {}

EvaluatorPtr generateEvaluator(const TemporalExpressionPtr &formula,
                               const TracePtr &trace) {

  return generatePtr<AutomataBasedEvaluator>(formula, trace);
}

} // namespace harm
