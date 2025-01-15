#include "EdgeProposition.hh"
#include "expUtils/Z3ExpWrapper.hh"
#include "expUtils/expUtils.hh"
#include "visitors/ExpToZ3Visitor.hh"
#include "z3TestCaseGenerator.hh"
#include <chrono>
#include <exp.hh>
#include <map>
#include <vector>
#include <z3++.h>

using namespace expression;

namespace z3 {
//PropositionPtr
//edgePropositionToProposition(harm::EdgeProposition *edge);

//bool check_implies(const expression::PropositionPtr &p1,
//                   const expression::PropositionPtr &p2) {
//  //make not p1 or p2
//  PropositionPtr not_p1_imp_p2 =
//      makeGenericExpression<PropositionNot>(
//          makeGenericExpression<PropositionImplication>(p1, p2));
//
//  Z3ExpWrapper z3_not_imp = expression::to_z3exp(not_p1_imp_p2);
//
//  //make z3 implication
//  z3::context *ctx = z3_not_imp._ctx.get();
//
//  z3::solver s(*ctx);
//  z3::params params(*ctx);
//
//  // max 1 second before aborting the solver
//  params.set(":timeout", 1000U);
//  s.set(params);
//  s.add(*z3_not_imp._exp);
//  //  std::cout << s << "\n";
//
//  return s.check() == unsat;
//}

bool check_equivalence(const expression::PropositionPtr &p1,
                       const expression::PropositionPtr &p2) {

  PropositionPtr not_p1_eq_p2 = makeGenericExpression<PropositionNot>(
      makeGenericExpression<PropositionEq>(p1, p2));

  Z3ExpWrapper z3_not_eq = expression::to_z3exp(not_p1_eq_p2);
  z3::context *ctx = z3_not_eq._ctx.get();

  z3::solver s(*ctx);
  z3::params params(*ctx);

  // max 1 second before aborting the solver
  params.set(":timeout", 1000U);
  s.set(params);

  s.add(*z3_not_eq._exp);

  return s.check() == unsat;
}

using Literal = std::string;

struct DNF_AND {
  std::unordered_set<Literal> _positive_literals;
  std::unordered_set<Literal> _negated_literals;
};

struct DNF_OR {
  std::vector<DNF_AND> _ands;
  std::string to_string() const {
    std::string res;
    for (const auto &and_ : _ands) {
      res += "(";
      for (const auto &lit : and_._positive_literals) {
        res += lit + " ";
      }
      for (const auto &lit : and_._negated_literals) {
        res += "!" + lit + " ";
      }
      res += ") ";
    }
    return res;
  }
};

// Check if one AND clause covers another
bool covers(const DNF_AND &candidate, const DNF_AND &target) {
  // Check if all positive and negated literals in target are also in candidate
  for (const auto &lit : target._positive_literals) {
    if (candidate._positive_literals.count(lit) == 0)
      return false;
  }
  for (const auto &lit : target._negated_literals) {
    if (candidate._negated_literals.count(lit) == 0)
      return false;
  }
  return true;
}

// Check if DNF p1 implies DNF p2
bool implies(const DNF_OR &p1, const DNF_OR &p2) {
  for (const auto &p1_and : p1._ands) {
    bool is_covered = false;
    for (const auto &p2_and : p2._ands) {
      if (covers(p1_and, p2_and)) {
        is_covered = true;
        break;
      }
    }
    if (!is_covered)
      return false;
  }
  return true;
}
DNF_AND to_dnf_and(harm::EdgeProposition *edge) {
  DNF_AND dnf_and;

  //edge is only one clause
  if (dynamic_cast<harm::EdgeAnd *>(edge) == nullptr) {
    if (dynamic_cast<harm::EdgeNot *>(edge)) {
      //negated literal
      dnf_and._negated_literals.insert(
          edge->getOperands()[0]->toString());
    } else {
      dnf_and._positive_literals.insert(edge->toString());
    }
    return dnf_and;
  } else {
    for (auto &ap_ : edge->getOperands()) {
      if (dynamic_cast<harm::EdgeNot *>(ap_)) {
        dnf_and._negated_literals.insert(
            ap_->getOperands()[0]->toString());
      } else {
        dnf_and._positive_literals.insert(ap_->toString());
      }
    }
  }
  return dnf_and;
}

DNF_OR to_dnf_or(harm::EdgeProposition *edge) {
  DNF_OR dnf_or;

  //edge is only one clause
  if (dynamic_cast<harm::EdgeOr *>(edge) == nullptr) {
    dnf_or._ands.push_back(to_dnf_and(edge));
    return dnf_or;
  }

  //edge is an or
  for (auto &and_ : edge->getOperands()) {
    dnf_or._ands.push_back(to_dnf_and(and_));
  }

  return dnf_or;
}

bool check_implies(harm::EdgeProposition *edge1,
                   harm::EdgeProposition *edge2) {

  //debug
  //std::cout << edge1->toString() << " =>\n\t" << edge2->toString()
  //          << "\n";

  //PropositionPtr p1 = edgePropositionToProposition(edge1);
  //PropositionPtr p2 = edgePropositionToProposition(edge2);

  //auto res1 = check_implies(p1, p2);

  DNF_OR dnf_or1 = to_dnf_or(edge1);
  DNF_OR dnf_or2 = to_dnf_or(edge2);

  //debug
  //std::cout << dnf_or1.to_string() << "\t" << dnf_or2.to_string()
  //          << "\n";
  //std::cout << "-----------------------"
  //          << "\n";

  //check if dnf_or1 implies dnf_or2
  bool res2 = implies(dnf_or1, dnf_or2);

  //messageErrorIf(res1 != res2, "Error: check_implies returned " +
  //                                 std::to_string(res1) +
  //                                 " but implies returned " +
  //                                 std::to_string(res2));
  return res2;

} // end fun

//PropositionPtr
//edgePropositionToProposition(harm::EdgeProposition *edge) {
//  //visit the expression depending on the type of operator
//
//  // And
//  if (isEdgeAnd(edge)) {
//    auto children = edge->getOperands();
//    auto and_ = makeGenericExpression<PropositionAnd>(
//        edgePropositionToProposition(children[0]),
//        edgePropositionToProposition(children[1]));
//    for (size_t i = 2; i < children.size(); i++) {
//      and_->addItem(edgePropositionToProposition(children[i]));
//    }
//
//    return and_;
//  }
//
//  // Or
//  if (isEdgeOr(edge)) {
//    auto children = edge->getOperands();
//    auto or_ = makeGenericExpression<PropositionOr>(
//        edgePropositionToProposition(children[0]),
//        edgePropositionToProposition(children[1]));
//    for (size_t i = 2; i < children.size(); i++) {
//      or_->addItem(edgePropositionToProposition(children[i]));
//    }
//    return or_;
//  }
//
//  // Not
//  if (isEdgeNot(edge)) {
//    return makeGenericExpression<PropositionNot>(
//        edgePropositionToProposition(edge->getOperands()[0]));
//  }
//
//  // Atomic proposition
//  if (isEdgeInst(edge)) {
//    return dynamic_cast<harm::EdgeInst *>(edge)->_toInst;
//  }
//
//  if (isEdgeTrue(edge)) {
//    return generatePtr<BooleanConstant>(true, ExpType::Bool, 1, -1);
//  }
//
//  if (isEdgeFalse(edge)) {
//    return generatePtr<BooleanConstant>(true, ExpType::Bool, 0, -1);
//  }
//
//  messageError("Unknown edge proposition: " + edge->toString());
//  return nullptr;
//}
} // namespace z3
