#ifndef SRC_EDIT_COSTS_BOOLEAN_AUTOMATON_HPP_
#define SRC_EDIT_COSTS_BOOLEAN_AUTOMATON_HPP_

#include "src/edit_costs/edit_costs.hpp"
#include "z3CheckSat.hh"
#include <map>
#include <string>
#include <unordered_map>

extern std::map<std::pair<std::string, std::string>, double>
    edge_rel_cost_map;

namespace harm {
class EdgeProposition;
} // namespace harm

namespace ged {

/*!
 * @brief Implements constant edit cost functions.
 */
template <class UserNodeLabel, class UserEdgeLabel>
class BooleanAutomaton
    : public EditCosts<UserNodeLabel, UserEdgeLabel> {

public:
  virtual ~BooleanAutomaton();

  /*!
	 * @brief Constructor.
	 * @param[in] node_ins_cost
	 * @param[in] node_del_cost
	 * @param[in] node_rel_cost
	 * @param[in] edge_ins_cost
	 * @param[in] edge_del_cost
	 * @param[in] edge_rel_cost
	 * @note Calling the constructor with the default arguments constructs uniform edit costs.
	 */
  BooleanAutomaton(
      const std::unordered_map<std::string, harm::EdgeProposition *>
          &edgeStrToProposition);

  virtual double
  node_ins_cost_fun(const UserNodeLabel &node_label) const final;

  virtual double
  node_del_cost_fun(const UserNodeLabel &node_label) const final;

  virtual double
  node_rel_cost_fun(const UserNodeLabel &node_label_1,
                    const UserNodeLabel &node_label_2) const final;

  virtual double
  edge_ins_cost_fun(const UserEdgeLabel &edge_label) const final;

  virtual double
  edge_del_cost_fun(const UserEdgeLabel &edge_label) const final;

  virtual double
  edge_rel_cost_fun(const UserEdgeLabel &edge_label_1,
                    const UserEdgeLabel &edge_label_2) const final;

private:
  double node_ins_cost_ = 1.f;
  double node_del_cost_ = 1.f;
  double node_rel_cost_ = 1.f;
  double edge_ins_cost_ = 1.f;
  double edge_del_cost_ = 1.f;
  //double edge_rel_cost_ = 1.f; not used
  std::unordered_map<std::string, harm::EdgeProposition *>
      edgeStrToProposition_;
};

} // namespace ged

#ifndef SRC_EDIT_COSTS_BOOLEAN_AUTOMATON_IPP_
#define SRC_EDIT_COSTS_BOOLEAN_AUTOMATON_IPP_

#include <string>
#include <unordered_map>

namespace ged {

template <class UserNodeLabel, class UserEdgeLabel>
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::~BooleanAutomaton() {}

template <class UserNodeLabel, class UserEdgeLabel>
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::BooleanAutomaton(
    const std::unordered_map<std::string, harm::EdgeProposition *>
        &edgeStrToProposition)
    : edgeStrToProposition_(edgeStrToProposition) {}

template <class UserNodeLabel, class UserEdgeLabel>
double
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::node_ins_cost_fun(
    const UserNodeLabel &node_label) const {
  return node_ins_cost_;
}

template <class UserNodeLabel, class UserEdgeLabel>
double
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::node_del_cost_fun(
    const UserNodeLabel &node_label) const {
  return node_del_cost_;
}

template <class UserNodeLabel, class UserEdgeLabel>
double
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::node_rel_cost_fun(
    const UserNodeLabel &node_label_1,
    const UserNodeLabel &node_label_2) const {
  if (node_label_1 != node_label_2) {
    return node_rel_cost_;
  }
  return 0.0;
}

template <class UserNodeLabel, class UserEdgeLabel>
double
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::edge_ins_cost_fun(
    const UserEdgeLabel &edge_label) const {
  return edge_ins_cost_;
}

template <class UserNodeLabel, class UserEdgeLabel>
double
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::edge_del_cost_fun(
    const UserEdgeLabel &edge_label) const {
  return edge_del_cost_;
}

template <class UserNodeLabel, class UserEdgeLabel>
double
BooleanAutomaton<UserNodeLabel, UserEdgeLabel>::edge_rel_cost_fun(
    const UserEdgeLabel &edge_label_1,
    const UserEdgeLabel &edge_label_2) const {

  std::pair<std::string, std::string> edge1_edge2_label =
      std::make_pair(edge_label_1, edge_label_2);
  std::pair<std::string, std::string> edge2_edge1_label =
      std::make_pair(edge_label_2, edge_label_1);

  if (edge_label_1 == edge_label_2) {
    return 0.0;
  }

  double edge1_edge2_cost =
      edge_rel_cost_map.count(edge1_edge2_label)
          ? edge_rel_cost_map.at(edge1_edge2_label)
          : -1;
  double edge2_edge1_cost =
      edge_rel_cost_map.count(edge2_edge1_label)
          ? edge_rel_cost_map.at(edge2_edge1_label)
          : -1;

  if (edge1_edge2_cost != -1 && edge2_edge1_cost != -1) {
    return edge1_edge2_cost + edge2_edge1_cost;
  }

  harm::EdgeProposition *p1 = edgeStrToProposition_.at(edge_label_1);
  harm::EdgeProposition *p2 = edgeStrToProposition_.at(edge_label_2);

  if (edge1_edge2_cost == -1) {
    if (z3::check_implies(p1, p2)) {
      //std::cout << edge_label_1 << " -> " << edge_label_2 << "\n";
      edge1_edge2_cost = 0.f;
    } else {
      edge1_edge2_cost = 0.5f;
    }
    edge_rel_cost_map[edge1_edge2_label] = edge1_edge2_cost;
  }
  if (edge2_edge1_cost == -1) {
    if (z3::check_implies(p2, p1)) {
      //std::cout << edge_label_2 << " -> " << edge_label_1 << "\n";
      edge2_edge1_cost = 0.f;
    } else {
      edge2_edge1_cost = 0.5f;
    }
    edge_rel_cost_map[edge2_edge1_label] = edge2_edge1_cost;
  }

  return edge1_edge2_cost + edge2_edge1_cost;
}

} // namespace ged

#endif /* SRC_EDIT_COSTS_BOOLEAN_AUTOMATON_IPP_ */

#endif /* SRC_EDIT_COSTS_BOOLEAN_AUTOMATON_HPP_ */
