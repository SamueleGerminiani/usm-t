#pragma once
#pragma once
#include "expUtils/Z3ExpWrapper.hh"
#include "formula/atom/Atom.hh"
#include <map>

#include "message.hh"
namespace harm {
class EdgeProposition;
}
namespace z3 {

bool check_implies(const expression::PropositionPtr &p1,
                   const expression::PropositionPtr &p2);

bool check_implies(harm::EdgeProposition *p1,
                   harm::EdgeProposition *p2);

bool check_equivalence(const expression::PropositionPtr &p1,
                       const expression::PropositionPtr &p2);
} // namespace z3
