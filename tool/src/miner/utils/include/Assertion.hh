#pragma once

#include "Language.hh"
#include "formula/temporal/TemporalExpression.hh"
#include <stddef.h>
#include <string>
#include <utility>

enum class Trinary;

namespace harm {
class TemplateImplication;
class Trace;
using TracePtr = std::shared_ptr<Trace>;
class Evaluator;
using EvaluatorPtr = std::shared_ptr<Evaluator>;

/*! \class Assertion
    \brief Class representing an assertion (it is just a snapshot of an instantiated template)
*/
class Assertion {
  static size_t idCounter;

public:
  Assertion(const expression::TemporalExpressionPtr &formula);

  ~Assertion();

  std::string toString(Language lang = Language::Unset) const;
  std::string toColoredString(Language lang = Language::Unset) const;

  expression::TemporalExpressionPtr _formula = nullptr;
  void enableEvaluation(const TracePtr &trace);
  bool holdsOnTrace();
  void changeTrace(const TracePtr &trace);

  size_t _id = (size_t)-1;

  TracePtr _trace = nullptr;
  EvaluatorPtr _automataEvaluator = nullptr;
};
//smart pointer
using AssertionPtr = std::shared_ptr<Assertion>;

} // namespace harm
