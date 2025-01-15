#pragma once
#include "expUtils/expUtils.hh"
#include "formula/temporal/TemporalExpression.hh"
#include "globals.hh"
#include <mutex>

enum class Trinary;
namespace harm {

using namespace expression;

//forward declarations
class Automaton;
class Trace;

/** \class Evaluator
    \brief abstract class for the evaluation of temporal formulas
*/
class Evaluator {
public:
  virtual ~Evaluator() = default;

  virtual void changeTrace(const harm::TracePtr &trace) = 0;
  virtual bool holdsOnTrace() = 0;

protected:
  Evaluator(const TemporalExpressionPtr &formula,
            const harm::TracePtr &trace);

  ///the formula to be evaluated
  TemporalExpressionPtr _formula;
  ///the trace on which the formula is evaluated
  harm::TracePtr _trace;
};

///pointer to an Evaluator
using EvaluatorPtr = std::shared_ptr<Evaluator>;

EvaluatorPtr
generateEvaluator(const TemporalExpressionPtr &formula,
                  const TracePtr &trace,
                  const std::string &evaluatorType = "None");

} // namespace harm

