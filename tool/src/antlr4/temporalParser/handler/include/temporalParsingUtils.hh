#pragma once
#include <string>

#include "TemplateImplication.hh"
#include <memory>

namespace harm {
//class TemplateImplication;
class Trace;
using TracePtr = std::shared_ptr<harm::Trace>;
} // namespace harm
namespace expression {
class TemporalExpression;
using TemporalExpressionPtr = std::shared_ptr<TemporalExpression>;
} // namespace expression

namespace hparser {

expression::TemporalExpressionPtr
parseTemporalExpression(std::string formula,
                        const harm::TracePtr &trace);

std::string
isSyntacticallyCorrectTemporalExpression(std::string formula,
                                         const harm::TracePtr &trace);
} // namespace hparser
