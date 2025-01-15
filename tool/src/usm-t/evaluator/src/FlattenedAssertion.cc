#include "FlattenedAssertion.hh"
#include "ProgressBar.hpp"
#include "formula/formula.hh"
#include "z3CheckSat.hh"
#include <optional>

#include <set>
#include <utility>

std::map<std::pair<std::string, std::string>, double>
    equivalence_cache;
namespace usmt {
using namespace harm;
using namespace expression;

void remapTargets(
    std::unordered_map<std::string, std::string> &targetToRemap,
    std::unordered_map<std::string, PropositionPtr>
        &all_rtarget_toProposition) {

  progresscpp::ParallelProgressBar pb;
  pb.addInstance(0, "Flattening specifications",
                 all_rtarget_toProposition.size(), 70);

  //find semantically equivalent propositions
  std::unordered_map<std::string, std::set<std::string>>
      semantically_equivalent;
  for (auto it1 = all_rtarget_toProposition.begin();
       it1 != all_rtarget_toProposition.end(); it1++) {
    auto &[rt1, p1] = *it1;
    if (p1 == nullptr) {
      pb.increment(0);
      pb.display();
      continue;
    } else {
      semantically_equivalent[rt1];
    }
    for (auto it2 = std::next(it1);
         it2 != all_rtarget_toProposition.end(); it2++) {
      auto &[rt2, p2] = *it2;
      if (p2 == nullptr || it1 == it2) {
        continue;
      }

      //syntax comparison for boolean variables
      if (isBooleanVariable(p1) && isBooleanVariable(p2)) {
        if (rt1 == rt2) {
          semantically_equivalent[rt1].insert(rt2);
          p2 = nullptr;
        } else {
          continue;
        }
      }

      bool is_equivalent = false;
      if (equivalence_cache.count({rt1, rt2})) {
        is_equivalent = equivalence_cache.at({rt1, rt2});
      } else {
        is_equivalent = z3::check_equivalence(p1, p2);
        equivalence_cache[std::make_pair(rt1, rt2)] = is_equivalent;
        equivalence_cache[std::make_pair(rt2, rt1)] = is_equivalent;
      }

      if (is_equivalent) {
        semantically_equivalent[rt1].insert(rt2);
        p2 = nullptr;
      }

    } //end for it2
    pb.increment(0);
    pb.display();
  }
  pb.done(0);

  //debug
  //print semantically equivalent
  //for (const auto &[rt, eqs] : semantically_equivalent) {
  //  std::cout << rt << " is semantically equivalent to: ";
  //  for (const auto &eq : eqs) {
  //    std::cout << eq << " ";
  //  }
  //  std::cout << "\n";
  //}
  //exit(0);

  size_t tokenID = 0;
  for (const auto &[representant, equivalents] :
       semantically_equivalent) {
    std::string newToken = "_inst_" + std::to_string(tokenID++);
    targetToRemap[representant] = newToken;
    for (auto eq : equivalents) {
      targetToRemap[eq] = newToken;
    }
  }
}

std::vector<BooleanLayerInstPtr>
getBooleaLayerInst(const AssertionPtr &a) {
  const TemporalExpressionPtr &te = a->_formula;
  std::vector<BooleanLayerInstPtr> ret;
  traverse(te, [&](const TemporalExpressionPtr &current) {
    if (isInst(current)) {
      ret.push_back(
          std::dynamic_pointer_cast<BooleanLayerInst>(current));
    }
    return false;
  });
  return ret;
}

void gatherRemapTargets(
    const std::vector<AssertionPtr> &assertions,
    std::unordered_map<std::string, PropositionPtr>
        &all_rtarget_toProposition) {

  //gather all remap targets--------------------------
  for (const auto &a : assertions) {
    auto insts = getBooleaLayerInst(a);
    for (const auto &i : insts) {
      PropositionPtr p = i->getProposition();
      std::unordered_map<std::string, PropositionPtr> rtargets =
          prop2RemapTargets(p);
      all_rtarget_toProposition.insert(rtargets.begin(),
                                       rtargets.end());
    }
  }
}

std::unordered_map<std::string, std::vector<FlattenedAssertion>>
getFlattenedAssertions(
    const std::vector<AssertionPtr> &expected_assertions,
    const std::vector<AssertionPtr> &mined_assertions,
    std::unordered_map<std::string, std::string> &targetToRemap) {

  std::unordered_map<std::string, std::vector<FlattenedAssertion>>
      ret;

  std::unordered_map<std::string, PropositionPtr>
      all_rtarget_toProposition;

  gatherRemapTargets(expected_assertions, all_rtarget_toProposition);
  gatherRemapTargets(mined_assertions, all_rtarget_toProposition);
  //debug
  //print gathered remap targets
  //for (const auto &[rt, p] : all_rtarget_toProposition) {
  //  std::cout << rt << " -> " << prop2ColoredString(p) << "\n";
  //}

  remapTargets(targetToRemap, all_rtarget_toProposition);

  //retrieve flattened expected assertions
  for (const auto &a : expected_assertions) {
    messageErrorIf(
        !isPropertyAlways(a->_formula),
        "Expecting a safety assertion starting with G, got '" +
            a->toString() + "'");
    auto flattened_str =
        temp2RemapString(a->_formula->getItems()[0], targetToRemap,
                         Language::SpotLTL, PrintMode::ShowAll);
    ret["expected"].push_back({a, flattened_str});
  }

  //retrieve flattened mined assertions
  for (const auto &a : mined_assertions) {
    messageErrorIf(
        !isPropertyAlways(a->_formula),
        "Expecting a safety assertion starting with G, got '" +
            a->toString() + "'");
    auto flattened_str =
        temp2RemapString(a->_formula->getItems()[0], targetToRemap,
                         Language::SpotLTL, PrintMode::ShowAll);
    ret["mined"].push_back({a, flattened_str});
  }

  return ret;
}
} // namespace usmt
