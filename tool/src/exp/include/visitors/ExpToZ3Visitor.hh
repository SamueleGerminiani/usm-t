
#pragma once

#include "expUtils/Z3ExpWrapper.hh"

#include "visitors/ExpVisitor.hh"

#include <algorithm>
#include <cmath>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace expression {

namespace ope {
enum ope : int;
}

class ExpToZ3Visitor : public ExpVisitor {
public:
  /// @brief Constructor.
  ExpToZ3Visitor();

  /// @brief Destructor.
  virtual ~ExpToZ3Visitor() override;

  Z3ExpWrapper get();

  void clear();
  size_t getNumberOfVariables();

  // proposition

  virtual void visit(BooleanConstant &o) override;
  virtual void visit(BooleanVariable &o) override;
  virtual void visit(PropositionAnd &o) override;
  virtual void visit(PropositionOr &o) override;
  virtual void visit(PropositionImplication &o) override;
  virtual void visit(PropositionXor &o) override;
  virtual void visit(PropositionEq &o) override;
  virtual void visit(PropositionNeq &o) override;
  virtual void visit(PropositionNot &o) override;
  virtual void visit(FloatConstant &o) override;
  virtual void visit(FloatVariable &o) override;
  virtual void visit(FloatSum &o) override;
  virtual void visit(FloatSub &o) override;
  virtual void visit(FloatMul &o) override;
  virtual void visit(FloatDiv &o) override;
  virtual void visit(FloatEq &o) override;
  virtual void visit(FloatNeq &o) override;
  virtual void visit(FloatGreater &o) override;
  virtual void visit(FloatGreaterEq &o) override;
  virtual void visit(FloatLess &o) override;
  virtual void visit(FloatLessEq &o) override;
  virtual void visit(FloatToBool &o) override;
  virtual void visit(FloatToInt &o) override;
  virtual void visit(IntConstant &o) override;
  virtual void visit(IntVariable &o) override;
  virtual void visit(IntSum &o) override;
  virtual void visit(IntSub &o) override;
  virtual void visit(IntMul &o) override;
  virtual void visit(IntDiv &o) override;
  virtual void visit(IntBAnd &o) override;
  virtual void visit(IntBOr &o) override;
  virtual void visit(IntBXor &o) override;
  virtual void visit(IntNot &o) override;
  virtual void visit(IntEq &o) override;
  virtual void visit(IntNeq &o) override;
  virtual void visit(IntGreater &o) override;
  virtual void visit(IntGreaterEq &o) override;
  virtual void visit(IntLess &o) override;
  virtual void visit(IntLessEq &o) override;
  //missing
  //virtual void visit(IntBitSelector &o) override;
  virtual void visit(IntToBool &o) override;
  virtual void visit(IntToFloat &o) override;
  virtual void visit(IntLShift &o) override;
  virtual void visit(IntRShift &o) override;

  std::pair<ExpType, size_t> getType(const std::string &varName) {
    if (_varNameToType.count(varName) == 0) {
      return std::make_pair(ExpType::Unknown, 0);
    } else {
      return _varNameToType.at(varName);
    }
  }

private:
  std::stack<z3::expr> _z3Expressions;
  Z3ContextPtr _c;
  z3::expr _overflowCond;
  std::unordered_map<std::string, std::pair<ExpType, size_t>>
      _varNameToType;
};

} // namespace expression
