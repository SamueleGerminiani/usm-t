#pragma once
#include <stddef.h>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "formula/expression/GenericExpression.hh"
#include "visitors/ExpVisitor.hh"

namespace expression {
enum class ExpType;

/// @brief This class extracts all the variables from an expression.
class RemapVisitor : public ExpVisitor {
public:
  RemapVisitor();
  ~RemapVisitor() override = default;

  /// @brief returns the list of targets that need remapping.
  std::unordered_map<std::string, PropositionPtr> get_remap_targets();

  void visit(BooleanVariable &o) override;

  virtual void visit(PropositionAnd &o) override;
  virtual void visit(PropositionOr &o) override;
  virtual void visit(PropositionXor &o) override;
  virtual void visit(PropositionEq &o) override;
  virtual void visit(PropositionNeq &o) override;
  virtual void visit(PropositionNot &o) override;

  virtual void visit(IntToBool &o) override;
  virtual void visit(FloatToBool &o) override;

  virtual void visit(FloatEq &o) override;
  virtual void visit(FloatNeq &o) override;
  virtual void visit(FloatGreater &o) override;
  virtual void visit(FloatGreaterEq &o) override;
  virtual void visit(FloatLess &o) override;
  virtual void visit(FloatLessEq &o) override;

  virtual void visit(IntEq &o) override;
  virtual void visit(IntNeq &o) override;
  virtual void visit(IntGreater &o) override;
  virtual void visit(IntGreaterEq &o) override;
  virtual void visit(IntLess &o) override;
  virtual void visit(IntLessEq &o) override;

  void setRootRawPointer(const PropositionPtr &root) {
    _rawPointerToSmart[root.get()] = root;
  }

protected:
  /// @brief list of targets than need remapping.
  std::unordered_map<std::string, PropositionPtr> _remapTargets;
  std::unordered_map<Proposition *, PropositionPtr>
      _rawPointerToSmart;
};

} // namespace expression
