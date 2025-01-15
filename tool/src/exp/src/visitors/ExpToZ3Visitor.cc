
#include "visitors/ExpToZ3Visitor.hh"
#include "expUtils/ExpType.hh"
#include "expUtils/implicitConversion.hh"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../miner/utils/include/DTLimits.hh"
#include "Int.hh"
#include "colors.hh"
#include "expUtils/ExpType.hh"

#include "expUtils/ope.hh"
#include "formula/atom/Atom.hh"
#include "formula/atom/Constant.hh"
#include "formula/atom/Variable.hh"
#include "formula/expression/BitSelector.hh"
#include "formula/expression/GenericExpression.hh"
#include "formula/expression/TypeCast.hh"
#include "globals.hh"
#include "misc.hh"
#include "visitors/PrinterVisitor.hh"
#include <cmath>
#include <iomanip>
#include <z3++.h>

namespace expression {

static double double_precision = 0.01;
static double double_limit = std::pow(2, 30);

#define EXTEND_LOGIC(O, OE1, OE2, E1, E2)                                        \
  size_t TO_EXTEND[2];                                                           \
  TO_EXTEND[0] = std::abs((int)OE1->getType().second -                           \
                          (int)O.getType().second);                              \
  TO_EXTEND[1] = std::abs((int)OE2->getType().second -                           \
                          (int)O.getType().second);                              \
  if (TO_EXTEND[0] > 0) {                                                        \
    /*If the operand is going from unsigned to signed, add a zero to           \
    avoid errors with z3::sext*/ \
    if (OE1->getType().first == ExpType::UInt) {                                 \
      E1 = z3::zext(E1, 1);                                                      \
      TO_EXTEND[0] = TO_EXTEND[0] - 1;                                           \
    }                                                                            \
    E1 = z3::sext(E1, TO_EXTEND[0]);                                             \
  }                                                                              \
  if (TO_EXTEND[1] > 0) {                                                        \
    if (OE2->getType().first == ExpType::UInt) {                                 \
      E2 = z3::zext(E2, 1);                                                      \
      TO_EXTEND[1] = TO_EXTEND[1] - 1;                                           \
    }                                                                            \
    E2 = z3::sext(E2, TO_EXTEND[1]);                                             \
  }

#define EXTEND_LOGIC_APPLY_CSTANDARD(O, OE1, OE2, E1, E2)                        \
  auto conversionResult = applyCStandardConversion(                              \
      std::make_pair(OE1->getType().first, OE1->getType().second),               \
      std::make_pair(OE2->getType().first, OE2->getType().second));              \
  size_t TO_EXTEND[2];                                                           \
  TO_EXTEND[0] = std::abs((int)OE1->getType().second -                           \
                          (int)conversionResult.second);                         \
  TO_EXTEND[1] = std::abs((int)OE2->getType().second -                           \
                          (int)conversionResult.second);                         \
  if (TO_EXTEND[0] > 0) {                                                        \
    /*If the operand is going from unsigned to signed, add a zero to           \
    avoid errors with z3::sext*/ \
    if (OE1->getType().first == ExpType::UInt) {                                 \
      E1 = z3::zext(E1, 1);                                                      \
      TO_EXTEND[0] = TO_EXTEND[0] - 1;                                           \
    }                                                                            \
    E1 = z3::sext(E1, TO_EXTEND[0]);                                             \
  }                                                                              \
  if (TO_EXTEND[1] > 0) {                                                        \
    if (OE2->getType().first == ExpType::UInt) {                                 \
      E2 = z3::zext(E2, 1);                                                      \
      TO_EXTEND[1] = TO_EXTEND[1] - 1;                                           \
    }                                                                            \
    E2 = z3::sext(E2, TO_EXTEND[1]);                                             \
  }

#define EXTEND_FLOAT(O, OE1, OE2, E1, E2)                            \
  bool TO_EXTEND[2];                                                 \
  TO_EXTEND[0] = (O.getType().second - OE1->getType().second) != 0;  \
  TO_EXTEND[1] = (O.getType().second - OE2->getType().second) != 0;  \
  z3::expr rm(*_c, _c->fpa_rounding_mode());                         \
  z3::sort srt = _c->fpa_sort(11, 53);                               \
  if (TO_EXTEND[0]) {                                                \
    E1 = z3::expr(*_c, Z3_mk_fpa_to_fp_float(*_c, rm, E1, srt));     \
  }                                                                  \
  if (TO_EXTEND[1]) {                                                \
    E2 = z3::expr(*_c, Z3_mk_fpa_to_fp_float(*_c, rm, E2, srt));     \
  }

#define EXTEND_FLOAT_APPLY_CSTANDARD(O, OE1, OE2, E1, E2)            \
  auto conversionResult = applyCStandardConversion(                  \
      std::make_pair(OE1->getType().first, OE1->getType().second),   \
      std::make_pair(OE2->getType().first, OE2->getType().second));  \
                                                                     \
  bool TO_EXTEND[2];                                                 \
  TO_EXTEND[0] =                                                     \
      (conversionResult.second - OE1->getType().second) != 0;        \
  TO_EXTEND[1] =                                                     \
      (conversionResult.second - OE2->getType().second) != 0;        \
                                                                     \
  z3::expr rm(*_c, _c->fpa_rounding_mode());                         \
  z3::sort srt = _c->fpa_sort(11, 53);                               \
  if (TO_EXTEND[0]) {                                                \
    E1 = z3::expr(*_c, Z3_mk_fpa_to_fp_float(*_c, rm, E1, srt));     \
  }                                                                  \
  if (TO_EXTEND[1]) {                                                \
    E2 = z3::expr(*_c, Z3_mk_fpa_to_fp_float(*_c, rm, E2, srt));     \
  }

ExpToZ3Visitor::~ExpToZ3Visitor() {}

ExpToZ3Visitor::ExpToZ3Visitor()
    : ExpVisitor(), _z3Expressions(),
      _c(std::make_shared<z3::context>()), _overflowCond(*_c),
      _varNameToType() {

  _overflowCond = _c->bool_val(true);
}

Z3ExpWrapper ExpToZ3Visitor::get() {
  assert(_z3Expressions.size() == 1);
  //    std::cout<<_overflowCond<<"\n";
  if (_overflowCond.is_const()) {
    return Z3ExpWrapper(_z3Expressions.top(), _c);
  } else {
    return Z3ExpWrapper(_z3Expressions.top() && _overflowCond, _c);
  }
}

size_t ExpToZ3Visitor::getNumberOfVariables() {
  return _varNameToType.size();
}

void ExpToZ3Visitor::clear() {
  for (size_t i = 0; i < _z3Expressions.size(); i++) {
    _z3Expressions.pop();
  }

  _overflowCond = _c->bool_val(true);
  _varNameToType.clear();
}

// proposition
void ExpToZ3Visitor::visit(expression::BooleanVariable &o) {
  assert(_varNameToType.count(o.getName()) == 0 ||
         (_varNameToType.at(o.getName()) ==
          std::make_pair(o.getType().first, o.getType().second)));
  _varNameToType[o.getName()] = o.getType();

  _z3Expressions.push(_c->bool_const(o.getName().c_str()));
}

void ExpToZ3Visitor::visit(expression::BooleanConstant &o) {
  if (o.evaluate(0)) {
    z3::expr trueConstant = _c->bool_val(true);
    _z3Expressions.push(trueConstant);
  } else {
    z3::expr falseConstant = _c->bool_val(false);
    _z3Expressions.push(falseConstant);
  }
}

void ExpToZ3Visitor::visit(expression::PropositionAnd &o) {
  z3::expr_vector expVector(*_c);

  //  assert(o.getItems().size() > 1);

  for (const auto &e : o.getItems()) {
    e->acceptVisitor(*this);
    expVector.push_back(_z3Expressions.top());
    _z3Expressions.pop();
  }
  _z3Expressions.push(z3::mk_and(expVector));
}

void ExpToZ3Visitor::visit(expression::PropositionOr &o) {
  z3::expr_vector expVector(*_c);

  assert(o.getItems().size() > 1);

  for (const auto &e : o.getItems()) {
    e->acceptVisitor(*this);
    expVector.push_back(_z3Expressions.top());
    _z3Expressions.pop();
  }
  _z3Expressions.push(z3::mk_or(expVector));
}
void ExpToZ3Visitor::visit(expression::PropositionImplication &o) {
  assert(o.getItems().size() == 2);

  o.getItems()[0]->acceptVisitor(*this);
  o.getItems()[1]->acceptVisitor(*this);

  //get ant and cons
  z3::expr con = _z3Expressions.top();
  _z3Expressions.pop();
  z3::expr ant = _z3Expressions.top();
  _z3Expressions.pop();
  _z3Expressions.push(z3::implies(ant, con));
}

void ExpToZ3Visitor::visit(expression::PropositionXor &o) {
  assert(o.getItems().size() == 2);
  o.getItems()[0]->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  o.getItems()[1]->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  _z3Expressions.push(e1 ^ e2);
}

void ExpToZ3Visitor::visit(expression::PropositionEq &o) {
  assert(o.getItems().size() == 2);
  o.getItems()[0]->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  o.getItems()[1]->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  _z3Expressions.push(e1 == e2);
}

void ExpToZ3Visitor::visit(expression::PropositionNeq &o) {
  assert(o.getItems().size() == 2);
  o.getItems()[0]->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  o.getItems()[1]->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  _z3Expressions.push(e1 != e2);
}

void ExpToZ3Visitor::visit(expression::PropositionNot &o) {
  assert(o.getItems().size() == 1);
  o.getItems()[0]->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  _z3Expressions.push(!e1);
}

void ExpToZ3Visitor::visit(expression::IntNot &o) {
  assert(o.getItems().size() == 1);
  o.getItems()[0]->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  _z3Expressions.push(~e1);
}

// numeric
void ExpToZ3Visitor::visit(expression::FloatVariable &o) {
  assert(_varNameToType.count(o.getName()) == 0 ||
         (_varNameToType.at(o.getName()) ==
          std::make_pair(o.getType().first, o.getType().second)));

  _varNameToType[o.getName()] = o.getType();

  if (o.getType().second == 64) {
    z3::expr var = _c->fpa_const(o.getName().c_str(), 11, 53);
    _z3Expressions.push(var);
    _overflowCond = _overflowCond && var != _c->fpa_val(std::nan(""));
    _overflowCond =
        _overflowCond &&
        var != _c->fpa_val(std::numeric_limits<double>::infinity());
    _overflowCond =
        _overflowCond &&
        var != _c->fpa_val(-std::numeric_limits<double>::infinity());
    _overflowCond = _overflowCond &&
                    (z3::abs(var) > _c->fpa_val(double_precision) ||
                     var == _c->fpa_val(.0));
    _overflowCond =
        _overflowCond && (z3::abs(var) < _c->fpa_val(double_limit));
  } else {
    messageError("Wrong floating-point size: expected 32 or 64!");
  }
}

void ExpToZ3Visitor::visit(expression::FloatConstant &o) {
  if (o.getType().second == 32) {
    _z3Expressions.push(_c->fpa_val((float)o.evaluate(0)));
  } else if (o.getType().second == 64) {
    _z3Expressions.push(_c->fpa_val((double)o.evaluate(0)));
  } else {
    messageError("Wrong floating-point size: expected 32 or 64!");
  }
}

void ExpToZ3Visitor::visit(expression::FloatSum &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT(o, oe1, oe2, e1, e2);

  z3::expr finalExpr = e1 + e2;

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::FloatSub &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT(o, oe1, oe2, e1, e2);
  z3::expr finalExpr = e1 - e2;

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::FloatMul &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT(o, oe1, oe2, e1, e2);

  z3::expr finalExpr = e1 * e2;

  _z3Expressions.push(finalExpr);
}
void ExpToZ3Visitor::visit(expression::FloatDiv &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  if (oe2->getType().second == 32) {
    _overflowCond = _overflowCond && (e2 != _c->fpa_val((float)0));
  } else {
    _overflowCond = _overflowCond && (e2 != _c->fpa_val((double)0));
  }

  EXTEND_FLOAT(o, oe1, oe2, e1, e2);

  z3::expr finalExpr = e1 / e2;

  _z3Expressions.push(finalExpr);
}
void ExpToZ3Visitor::visit(expression::FloatEq &o) {
  assert(o.getItems().size() == 2);
  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT_APPLY_CSTANDARD(o, oe1, oe2, e1, e2);

  _z3Expressions.push(e1 == e2);
}
void ExpToZ3Visitor::visit(expression::FloatNeq &o) {
  assert(o.getItems().size() == 2);
  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT_APPLY_CSTANDARD(o, oe1, oe2, e1, e2);

  _z3Expressions.push(e1 != e2);
}

void ExpToZ3Visitor::visit(expression::FloatGreater &o) {
  assert(o.getItems().size() == 2);
  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT_APPLY_CSTANDARD(o, oe1, oe2, e1, e2);

  _z3Expressions.push(e1 > e2);
}

void ExpToZ3Visitor::visit(expression::FloatGreaterEq &o) {
  assert(o.getItems().size() == 2);
  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT_APPLY_CSTANDARD(o, oe1, oe2, e1, e2);

  _z3Expressions.push(e1 >= e2);
}

void ExpToZ3Visitor::visit(expression::FloatLess &o) {
  assert(o.getItems().size() == 2);
  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT_APPLY_CSTANDARD(o, oe1, oe2, e1, e2);

  _z3Expressions.push(e1 < e2);
}

void ExpToZ3Visitor::visit(expression::FloatLessEq &o) {
  assert(o.getItems().size() == 2);
  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_FLOAT_APPLY_CSTANDARD(o, oe1, oe2, e1, e2);

  _z3Expressions.push(e1 <= e2);
}

void ExpToZ3Visitor::visit(expression::FloatToBool &o) {
  o.getItem()->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  _z3Expressions.push(e1 > 0);
}

// logic
void ExpToZ3Visitor::visit(expression::IntVariable &o) {
  assert(_varNameToType.count(o.getName()) == 0 ||
         (_varNameToType.at(o.getName()) ==
          std::make_pair(o.getType().first, o.getType().second)));

  _varNameToType[o.getName()] =
      std::make_pair(o.getType().first, o.getType().second);

  _z3Expressions.push(
      _c->bv_const(o.getName().c_str(), o.getType().second));
}

void ExpToZ3Visitor::visit(expression::IntConstant &o) {
  _z3Expressions.push(
      _c->bv_val((uint64_t)o.evaluate(0), o.getType().second));
}
void ExpToZ3Visitor::visit(expression::IntSum &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 + e2;

  // Add over/underflow conditions
  if (o.getType().first == ExpType::SInt) {
    _overflowCond =
        _overflowCond && z3::bvadd_no_overflow(e1, e2, true);
    _overflowCond = _overflowCond && z3::bvadd_no_underflow(e1, e2);
  } else {
    _overflowCond =
        _overflowCond && z3::bvadd_no_overflow(e1, e2, false);
    _overflowCond = _overflowCond && z3::bvadd_no_underflow(e1, e2);
  }

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::IntSub &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 - e2;

  // Add over/underflow conditions
  if (o.getType().first == ExpType::SInt) {
    _overflowCond = _overflowCond && z3::bvsub_no_overflow(e1, e2);
    _overflowCond =
        _overflowCond && z3::bvsub_no_underflow(e1, e2, true);
  } else {
    _overflowCond = _overflowCond && z3::bvsub_no_overflow(e1, e2);
    _overflowCond =
        _overflowCond && z3::bvsub_no_underflow(e1, e2, false);
  }

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::IntMul &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 * e2;

  if (o.getType().first == ExpType::SInt) {
    // Add over/underflow conditions
    _overflowCond =
        _overflowCond && z3::bvmul_no_overflow(e1, e2, true);
    _overflowCond = _overflowCond && z3::bvmul_no_underflow(e1, e2);
  } else {
    // Add over/underflow conditions
    _overflowCond =
        _overflowCond && z3::bvmul_no_overflow(e1, e2, false);
    _overflowCond = _overflowCond && z3::bvmul_no_underflow(e1, e2);
  }

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::IntDiv &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  // add the condition before extending the size
  _overflowCond =
      _overflowCond && (e2 != _c->bv_val(0, oe2->getType().second));

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 / e2;

  // Add over/underflow conditions
  if (o.getType().first == ExpType::SInt) {
    _overflowCond = _overflowCond && z3::bvsdiv_no_overflow(e1, e2);
  }

  _z3Expressions.push(finalExpr);
}
void ExpToZ3Visitor::visit(expression::IntEq &o) {
  assert(o.getItems().size() == 2);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC_APPLY_CSTANDARD(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = (e1 == e2);

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::IntNeq &o) {
  assert(o.getItems().size() == 2);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC_APPLY_CSTANDARD(o, oe1, oe2, e1, e2)

  _z3Expressions.push(e1 != e2);
}

void ExpToZ3Visitor::visit(expression::IntGreater &o) {
  assert(o.getItems().size() == 2);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC_APPLY_CSTANDARD(o, oe1, oe2, e1, e2)

  if (conversionResult.first == ExpType::SInt) {
    _z3Expressions.push(e1 > e2);
  } else {
    _z3Expressions.push(z3::ugt(e1, e2));
  }
}

void ExpToZ3Visitor::visit(expression::IntGreaterEq &o) {
  assert(o.getItems().size() == 2);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_LOGIC_APPLY_CSTANDARD(o, oe1, oe2, e1, e2)

  if (conversionResult.first == ExpType::SInt) {
    _z3Expressions.push(e1 >= e2);
  } else {
    _z3Expressions.push(z3::uge(e1, e2));
  }
}

void ExpToZ3Visitor::visit(expression::IntLess &o) {
  assert(o.getItems().size() == 2);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_LOGIC_APPLY_CSTANDARD(o, oe1, oe2, e1, e2)

  if (conversionResult.first == ExpType::SInt) {
    _z3Expressions.push(e1 < e2);
  } else {
    _z3Expressions.push(z3::ult(e1, e2));
  }
}

void ExpToZ3Visitor::visit(expression::IntLessEq &o) {
  assert(o.getItems().size() == 2);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  EXTEND_LOGIC_APPLY_CSTANDARD(o, oe1, oe2, e1, e2)

  if (conversionResult.first == ExpType::SInt) {
    _z3Expressions.push(e1 <= e2);
  } else {
    _z3Expressions.push(z3::ule(e1, e2));
  }
}

void ExpToZ3Visitor::visit(expression::IntToBool &o) {
  o.getItem()->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  _z3Expressions.push(e1 > 0);
}

void ExpToZ3Visitor::visit(expression::IntBAnd &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 & e2;

  _z3Expressions.push(finalExpr);
}
void ExpToZ3Visitor::visit(expression::IntLShift &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = z3::shl(e1, e2);

  _z3Expressions.push(finalExpr);
}
void ExpToZ3Visitor::visit(expression::IntRShift &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = z3::ashr(e1, e2);

  _z3Expressions.push(finalExpr);
}
void ExpToZ3Visitor::visit(expression::IntBOr &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 | e2;

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::IntBXor &o) {
  assert(o.getItems().size() > 1);

  auto &oe1 = o.getItems()[0];
  auto &oe2 = o.getItems()[1];

  oe1->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();

  oe2->acceptVisitor(*this);
  z3::expr e2 = _z3Expressions.top();
  _z3Expressions.pop();

  /*Extend the size of the operands to make all operands of the same size (if
   * needed)*/
  EXTEND_LOGIC(o, oe1, oe2, e1, e2)

  z3::expr finalExpr = e1 ^ e2;

  _z3Expressions.push(finalExpr);
}

void ExpToZ3Visitor::visit(expression::IntToFloat &o) {
  auto &oe = o.getItem();
  oe->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  Z3_ast floatExpr;
  z3::expr rm(*_c, _c->fpa_rounding_mode());
  z3::sort srt = _c->fpa_sort(11, 53);
  if (oe->getType().first == ExpType::SInt) {
    floatExpr = Z3_mk_fpa_to_fp_signed(*_c, rm, e1, srt);
  } else {
    floatExpr = Z3_mk_fpa_to_fp_unsigned(*_c, rm, e1, srt);
  }
  _z3Expressions.push(z3::expr(*_c, floatExpr));
}

void ExpToZ3Visitor::visit(expression::FloatToInt &o) {
  messageError("?");
  auto &oe = o.getItem();
  oe->acceptVisitor(*this);
  z3::expr e1 = _z3Expressions.top();
  _z3Expressions.pop();
  //convert the float to int
  z3::expr intExpr = z3::round_fpa_to_closest_integer(e1);
  _z3Expressions.push(z3::expr(*_c, intExpr));
}

} // namespace expression
