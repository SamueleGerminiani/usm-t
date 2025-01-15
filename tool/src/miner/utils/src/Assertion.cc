#include <algorithm>

#include "Assertion.hh"
#include "DTOperator.hh"
#include "Evaluator.hh"
#include "TemplateImplication.hh"
#include "Trinary.hh"

#include "expUtils/expUtils.hh"
#include "globals.hh"

namespace harm {
size_t Assertion::idCounter = 0;

Assertion::Assertion(const expression::TemporalExpressionPtr &formula)
    : _formula(formula), _id(idCounter++) {}

Assertion::~Assertion() {}

std::string Assertion::toString(Language lang) const {
  lang = (lang == Language::Unset) ? clc::outputLang : lang;
  return temp2String(_formula, lang, PrintMode::ShowAll);
}

std::string Assertion::toColoredString(Language lang) const {
  lang = (lang == Language::Unset) ? clc::outputLang : lang;
  return temp2ColoredString(_formula, lang, PrintMode::ShowAll);
}
void Assertion::enableEvaluation(const TracePtr &trace) {
  _trace = trace;
  _automataEvaluator = generateEvaluator(_formula, _trace);
};
bool Assertion::holdsOnTrace() {
  messageErrorIf(_automataEvaluator == nullptr,
                 "Evaluation not enabled in assertion '" +
                     toString() + "'");
  return _automataEvaluator->holdsOnTrace();
}
void Assertion::changeTrace(const TracePtr &trace) {
  messageErrorIf(_automataEvaluator == nullptr,
                 "Evaluation not enabled in assertion '" +
                     toString() + "'");
  _automataEvaluator->changeTrace(trace);
}

} // namespace harm
