#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "Evaluator.hh"
#include "ProgressBar.hpp"
#include "TemplateImplication.hh"
#include "Trace.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "propositionParsingUtils.hh"
#include "src/env/ged_env.hpp"
#include "temporalParsingUtils.hh"
#include "usmt_evaluator.hh"
#include "visitors/ExpToZ3Visitor.hh"
#include "z3TestCaseGenerator.hh"
#include "gtest/gtest_pred_impl.h"
#include <chrono>

using namespace harm;

TEST(gedlibTests, gl1) {

  // Instantiate GED environment
  ged::GEDEnv<int, double, std::string> env;

  // Set constant edit costs (insert, delete, substitute)
  env.set_edit_costs(ged::Options::EditCosts::CONSTANT);

  // Add the first graph
  ged::GEDGraph::GraphID graph1 = env.add_graph("Graph1", "ClassA");
  env.add_node(graph1, 0, 1);
  env.add_node(graph1, 1, 1);
  env.add_node(graph1, 2, 1);
  env.add_node(graph1, 3, 1);
  env.add_node(graph1, 4, 1);
  env.add_node(graph1, 5, 1);
  env.add_node(graph1, 6, 1);
  env.add_node(graph1, 7, 0);
  env.add_edge(graph1, 5, 4, "1");
  env.add_edge(graph1, 4, 3, "a");
  env.add_edge(graph1, 4, 6, "!a");
  env.add_edge(graph1, 3, 2, "1");
  env.add_edge(graph1, 2, 6, "!b");
  env.add_edge(graph1, 2, 1, "b");
  env.add_edge(graph1, 1, 0, "1");
  env.add_edge(graph1, 0, 7, "!c");

  // Add the second graph
  ged::GEDGraph::GraphID graph2 = env.add_graph("Graph2", "ClassB");
  env.add_node(graph2, 0, 1);
  env.add_node(graph2, 1, 1);
  env.add_node(graph2, 2, 1);
  env.add_node(graph2, 3, 1);
  env.add_node(graph2, 4, 1);
  env.add_node(graph2, 5, 1);
  env.add_node(graph2, 6, 1);
  env.add_node(graph2, 7, 0);
  env.add_edge(graph2, 5, 4, "1");
  env.add_edge(graph2, 4, 3, "b");
  env.add_edge(graph2, 4, 6, "!b");
  env.add_edge(graph2, 3, 2, "1");
  env.add_edge(graph2, 2, 6, "!a");
  env.add_edge(graph2, 2, 1, "a");
  env.add_edge(graph2, 1, 0, "1");
  env.add_edge(graph2, 0, 7, "!c");

  // Initialize the environment
  env.init(ged::Options::InitType::EAGER_WITHOUT_SHUFFLED_COPIES);

  // Set the GED computation method
  env.set_method(ged::Options::GEDMethod::STAR,
                 "--optimal TRUE --threads 1");

  // Initialize the selected method
  env.init_method();

  // Run GED computation between the two graphs
  env.run_method(graph1, graph2, 0);

  // Retrieve and print the results
  double lower_bound = env.get_lower_bound(graph1, graph2);
  double upper_bound = env.get_upper_bound(graph1, graph2);

  std::cout
      << "Graph Edit Distance (Cost) between Graph1 and Graph2:\n";
  std::cout << "Lower Bound: " << lower_bound << "\n";
  std::cout << "Upper Bound: " << upper_bound << "\n";

  std::cout << "Node Mapping (Graph1 -> Graph2):" << std::endl;
  const ged::NodeMap &node_map = env.get_node_map(graph1, graph2);
  std::cout << node_map << "\n";
}
