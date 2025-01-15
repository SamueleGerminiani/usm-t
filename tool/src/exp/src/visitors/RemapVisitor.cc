#include "visitors/RemapVisitor.hh"
#include "expUtils/expUtils.hh"
#include "formula/atom/Atom.hh"
#include "formula/atom/Constant.hh"
#include "formula/atom/Variable.hh"
#include "formula/expression/BitSelector.hh"
#include "formula/expression/GenericExpression.hh"
#include "formula/expression/TypeCast.hh"
#include "formula/temporal/Property.hh"

#define VARIABLE(LEAF)                                               \
  void RemapVisitor::visit(LEAF &o) {                                \
    _remapTargets[o.getName()] = _rawPointerToSmart.at(&o);          \
  }

#define NUMERIC_TO_BOOL(NODE)                                        \
  void RemapVisitor::visit(expression::NODE &o) {                    \
    _remapTargets[prop2String(_rawPointerToSmart.at(&o))] =          \
        _rawPointerToSmart.at(&o);                                   \
  }

#define BOOL_WITH_CHILDREN(NODE)                                     \
  void RemapVisitor::visit(expression::NODE &o) {                    \
    for (const auto &child : o.getItems()) {                         \
      _rawPointerToSmart[child.get()] = child;                       \
      child->acceptVisitor(*this);                                   \
    }                                                                \
  }

namespace expression {

RemapVisitor::RemapVisitor() : ExpVisitor() {}

// proposition
VARIABLE(BooleanVariable)

std::unordered_map<std::string, PropositionPtr>
RemapVisitor::get_remap_targets() {
  _rawPointerToSmart.clear();
  return _remapTargets;
}

//make macro for greater, less,...
BOOL_WITH_CHILDREN(PropositionAnd)
BOOL_WITH_CHILDREN(PropositionOr)
BOOL_WITH_CHILDREN(PropositionXor)
BOOL_WITH_CHILDREN(PropositionEq)
BOOL_WITH_CHILDREN(PropositionNeq)
BOOL_WITH_CHILDREN(PropositionNot)

NUMERIC_TO_BOOL(FloatToBool)
NUMERIC_TO_BOOL(IntToBool)

NUMERIC_TO_BOOL(FloatEq)
NUMERIC_TO_BOOL(FloatNeq)
NUMERIC_TO_BOOL(FloatGreater)
NUMERIC_TO_BOOL(FloatGreaterEq)
NUMERIC_TO_BOOL(FloatLess)
NUMERIC_TO_BOOL(FloatLessEq)

NUMERIC_TO_BOOL(IntEq)
NUMERIC_TO_BOOL(IntNeq)
NUMERIC_TO_BOOL(IntGreater)
NUMERIC_TO_BOOL(IntGreaterEq)
NUMERIC_TO_BOOL(IntLess)
NUMERIC_TO_BOOL(IntLessEq)

} // namespace expression
