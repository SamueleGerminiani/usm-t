#include <filesystem>
#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

#include "Assertion.hh"
#include "AutomataBasedEvaluator.hh"
#include "CSVtraceReader.hh"
#include "EvalReport.hh"
#include "Evaluator.hh"
#include "ProgressBar.hpp"
#include "SimplifiedAssertion.hh"
#include "TemplateImplication.hh"
#include "Trace.hh"
#include "TraceReader.hh"
#include "assertion_utils.hh"
#include "globals.hh"
#include "message.hh"
#include "misc.hh"
#include "propositionParsingUtils.hh"
#include "temporalParsingUtils.hh"
#include "usmt_evaluator.hh"
#include "visitors/ExpToZ3Visitor.hh"
#include "z3TestCaseGenerator.hh"
#include "gtest/gtest_pred_impl.h"
#include <chrono>
#include <z3++.h>

#include <csignal>
#include <cstdio>
#include <fstream>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>

using namespace harm;
using namespace usmt;
using TraceReaderPtr = std::shared_ptr<TraceReader>;
namespace {
TracePtr generateMockTrace(size_t number_of_variables) {

  std::vector<VarDeclaration> vars;
  for (size_t i = 0; i < number_of_variables; i++) {
    vars.emplace_back("b_" + std::to_string(i), ExpType::Bool, 1);
    vars.emplace_back("i_" + std::to_string(i), ExpType::UInt, 32);
  }

  return generatePtr<Trace>(vars, 1);
}
std::vector<AssertionPtr> makeAssertionsFromTemplate(
    std::string _template, size_t max_ant_length,
    size_t max_con_length, size_t number_to_make,
    size_t max_number_of_variables, bool force_uint) {
  std::vector<AssertionPtr> ret;
  TracePtr trace = generateMockTrace(max_number_of_variables);
  std::vector<VarDeclaration> all_vars = trace->getVariables();
  if (force_uint) {
    all_vars = trace->getUnsignedIntVariabes();
  } else {
    all_vars = trace->getBooleanVariabes();
  }

  const std::string assertion_prefix = "G(";
  const std::string assertion_postfix = ")";

  if (_template == "..&&..") {
    for (size_t i = 0; i < number_to_make; i++) {
      //make antecedent
      std::string assertionStr_tmp = assertion_prefix;
      size_t ant_length = rand() % max_ant_length + 1;
      for (size_t j = 0; j < ant_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }
        if (j != ant_length - 1) {
          assertionStr_tmp += " && ";
        }
      }
      assertionStr_tmp += " -> ";
      //make consequent
      size_t on_length = rand() % max_con_length + 1;
      for (size_t j = 0; j < on_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }
        if (j != on_length - 1) {
          assertionStr_tmp += " && ";
        }
      }

      assertionStr_tmp += assertion_postfix;

      //debug
      //std::cout << "Assertion: " << assertionStr_tmp << "\n";
      ret.push_back(makeAssertion(assertionStr_tmp, trace));
    }
  } else if (_template == "..##1..") {
    for (size_t i = 0; i < number_to_make; i++) {
      //make antecedent
      std::string assertionStr_tmp = assertion_prefix + "{";
      size_t ant_length = rand() % max_ant_length + 1;
      for (size_t j = 0; j < ant_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }

        if (j != ant_length - 1) {
          assertionStr_tmp += " ##1 ";
        }
      }
      assertionStr_tmp += "} |-> ";
      //make consequent
      size_t on_length = rand() % max_con_length + 1;
      for (size_t j = 0; j < on_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";
        } else {
          assertionStr_tmp += randomVarName;
        }

        if (j != on_length - 1) {
          assertionStr_tmp += " ##1 ";
        }
      }

      assertionStr_tmp += assertion_postfix;

      //debug
      //std::cout << "Assertion: " << assertionStr_tmp << "\n";
      ret.push_back(makeAssertion(assertionStr_tmp, trace));
    }
  } else if (_template == "until" || _template == "eventually") {
    // G(a1 && .. && ak -> (c1 && .. && cj) U (d1 && .. && dh))
    // G(a1 && .. && ak -> F(c1 && .. && cj))
    auto makeConjunction = [&](size_t max_length) {
      std::string conj;
      size_t length = rand() % max_length + 1;
      for (size_t j = 0; j < length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          conj += "(" + randomVarName + " + " + randomVarName + ") > 0";
        } else {
          conj += randomVarName;
        }
        if (j != length - 1) {
          conj += " && ";
        }
      }
      return conj;
    };
    for (size_t i = 0; i < number_to_make; i++) {
      std::string assertionStr_tmp = assertion_prefix;
      assertionStr_tmp += makeConjunction(max_ant_length);
      assertionStr_tmp += " -> ";
      if (_template == "until") {
        assertionStr_tmp += "(" + makeConjunction(max_con_length) + ")";
        assertionStr_tmp += " U ";
        assertionStr_tmp += "(" + makeConjunction(max_con_length) + ")";
      } else {
        assertionStr_tmp +=
            "F(" + makeConjunction(max_con_length) + ")";
      }
      assertionStr_tmp += assertion_postfix;

      //debug
      //std::cout << "Assertion: " << assertionStr_tmp << "\n";
      ret.push_back(makeAssertion(assertionStr_tmp, trace));
    }
  } else if (_template.size() > 6 && _template.substr(0, 4) == "..##" &&
             _template.substr(_template.size() - 2) == "..") {
    // same as ..##1.. but with a generic delay ..##D..
    const std::string delay =
        _template.substr(4, _template.size() - 6);
    const std::string sep = " ##" + delay + " ";
    for (size_t i = 0; i < number_to_make; i++) {
      //make antecedent
      std::string assertionStr_tmp = assertion_prefix + "{";
      size_t ant_length = rand() % max_ant_length + 1;
      for (size_t j = 0; j < ant_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";

        } else {
          assertionStr_tmp += randomVarName;
        }

        if (j != ant_length - 1) {
          assertionStr_tmp += sep;
        }
      }
      assertionStr_tmp += "} |-> ";
      //make consequent
      size_t on_length = rand() % max_con_length + 1;
      for (size_t j = 0; j < on_length; j++) {
        std::string randomVarName =
            all_vars[rand() % all_vars.size()].getName();
        if (force_uint) {
          assertionStr_tmp +=
              "(" + randomVarName + " + " + randomVarName + ") > 0";
        } else {
          assertionStr_tmp += randomVarName;
        }

        if (j != on_length - 1) {
          assertionStr_tmp += sep;
        }
      }

      assertionStr_tmp += assertion_postfix;

      //debug
      //std::cout << "Assertion: " << assertionStr_tmp << "\n";
      ret.push_back(makeAssertion(assertionStr_tmp, trace));
    }
  }
  return ret;
}

} // namespace

extern std::map<std::pair<std::string, std::string>, double>
    edge_rel_cost_map;
extern std::map<std::pair<std::string, std::string>, double>
    equivalence_cache;

namespace {
struct AutomatonStats {
  size_t max_states = 0;
  double mean_states = 0;
  size_t max_edges = 0;
  double mean_edges = 0;
  size_t n_translated = 0;
  size_t n_ignored = 0;
};

// Size of the automata used by the hybrid similarity: same input string
// (the simplified/flattened assertion) and same translation
// (generateDeterministicSpotAutomaton) as evaluateWithHybrid
AutomatonStats
computeAutomatonStats(const std::vector<SimplifiedAssertion> &sas) {
  AutomatonStats stats;
  size_t tot_states = 0;
  size_t tot_edges = 0;
  for (const auto &sa : sas) {
    try {
      spot::formula f = spot::parse_infix_psl(sa.flattened_str).f;
      auto aut = generateDeterministicSpotAutomaton(f);
      size_t s = aut->num_states();
      size_t e = aut->num_edges();
      stats.max_states = std::max(stats.max_states, s);
      stats.max_edges = std::max(stats.max_edges, e);
      tot_states += s;
      tot_edges += e;
      stats.n_translated++;
    } catch (const std::exception &e) {
      // ignored by the hybrid similarity as well
      stats.n_ignored++;
    }
  }
  if (stats.n_translated > 0) {
    stats.mean_states = (double)tot_states / stats.n_translated;
    stats.mean_edges = (double)tot_edges / stats.n_translated;
  }
  return stats;
}

size_t test_with_parameters(std::string template_,
                            size_t number_of_assertions,
                            size_t number_of_variables,
                            bool force_uint, size_t max_ant_length = 3,
                            size_t max_con_length = 3,
                            AutomatonStats *aut_stats = nullptr) {
  edge_rel_cost_map.clear();
  equivalence_cache.clear();
  clearLanguageEquivalenceCache();

  std::vector<AssertionPtr> assertions = makeAssertionsFromTemplate(
      template_, max_ant_length, max_con_length, number_of_assertions,
      number_of_variables, force_uint);
  std::cout << "Example: " << assertions.front()->toString() << "\n";
  std::unordered_map<std::string, std::vector<AssertionPtr>>
      assertions_map;
  assertions_map["expected"] = assertions;
  assertions_map["mined"] = assertions;

  HybridReportPtr report = std::make_shared<HybridReport>();

  std::cout << "Number of comparisons: "
            << assertions_map["expected"].size() *
                   (assertions_map["mined"].size())
            << "\n";
  std::unordered_map<std::string, std::string> targetToRemap;
  auto tmp = getSimplifiedAssertions(assertions_map.at("expected"),
                                     assertions_map.at("mined"),
                                     targetToRemap);
  auto start = std::chrono::high_resolution_clock::now();
  evaluateWithHybrid(report, assertions_map);
  auto end = std::chrono::high_resolution_clock::now();
  // outside the timed region
  if (aut_stats != nullptr) {
    *aut_stats = computeAutomatonStats(tmp.at("expected"));
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                               start)
      .count();
}

// Same generation and simplification as test_with_parameters, without
// running the evaluator: used to report the automaton size of the
// configurations that do not complete
AutomatonStats automatonStatsOnly(std::string template_,
                                  size_t number_of_assertions,
                                  size_t number_of_variables,
                                  bool force_uint, size_t max_ant_length,
                                  size_t max_con_length) {
  std::vector<AssertionPtr> assertions = makeAssertionsFromTemplate(
      template_, max_ant_length, max_con_length, number_of_assertions,
      number_of_variables, force_uint);
  std::unordered_map<std::string, std::string> targetToRemap;
  auto tmp = getSimplifiedAssertions(assertions, assertions, targetToRemap);
  return computeAutomatonStats(tmp.at("expected"));
}

} // namespace

namespace {

// memory limit of each configuration (the machine has 32 GB)
const rlim_t memory_limit_bytes = 26ull * 1024 * 1024 * 1024;

struct FamilyConfig {
  std::string name;      // template name in the csv
  std::string family;    // challenge family
  std::string template_; // template understood by makeAssertionsFromTemplate
  size_t max_chain_length;
  size_t delay; // 0 if not applicable
};

struct RunResult {
  std::string status; // ok / timeout / oom / error
  size_t time_ms = 0;
  AutomatonStats stats;
};

// Runs one configuration in a child process, so that it can be killed if
// it exceeds the timeout or the memory limit. If stats_only, the evaluator
// is not run and only the automaton size is computed
RunResult runWithTimeout(const FamilyConfig &fc, size_t n_specs,
                         size_t n_vars, unsigned int seed,
                         size_t timeout_s, bool stats_only = false) {
  RunResult res;
  int fd[2];
  if (pipe(fd) != 0) {
    res.status = "error";
    return res;
  }
  std::cout.flush();
  pid_t pid = fork();
  if (pid == 0) {
    close(fd[0]);
    struct rlimit rl;
    rl.rlim_cur = memory_limit_bytes;
    rl.rlim_max = memory_limit_bytes;
    setrlimit(RLIMIT_AS, &rl);
    srand(seed);
    AutomatonStats stats;
    size_t t = 0;
    try {
      if (stats_only) {
        stats = automatonStatsOnly(fc.template_, n_specs, n_vars, false,
                                   fc.max_chain_length,
                                   fc.max_chain_length);
      } else {
        t = test_with_parameters(fc.template_, n_specs, n_vars, false,
                                 fc.max_chain_length,
                                 fc.max_chain_length, &stats);
      }
    } catch (const std::bad_alloc &e) {
      std::cout << "Out of memory\n";
      std::cout.flush();
      _exit(3);
    }
    std::string out = std::to_string(t) + " " +
                      std::to_string(stats.max_states) + " " +
                      std::to_string(stats.mean_states) + " " +
                      std::to_string(stats.max_edges) + " " +
                      std::to_string(stats.mean_edges) + " " +
                      std::to_string(stats.n_translated) + " " +
                      std::to_string(stats.n_ignored) + "\n";
    ssize_t w = write(fd[1], out.c_str(), out.size());
    (void)w;
    close(fd[1]);
    std::cout.flush();
    _exit(0);
  }
  close(fd[1]);
  if (pid < 0) {
    close(fd[0]);
    res.status = "error";
    return res;
  }

  auto start = std::chrono::steady_clock::now();
  int wstatus = 0;
  bool finished = false;
  while (true) {
    pid_t r = waitpid(pid, &wstatus, WNOHANG);
    if (r == pid) {
      finished = true;
      break;
    }
    if (std::chrono::steady_clock::now() - start >
        std::chrono::seconds(timeout_s)) {
      kill(pid, SIGKILL);
      waitpid(pid, &wstatus, 0);
      break;
    }
    usleep(200000);
  }

  if (!finished) {
    close(fd[0]);
    res.status = "timeout";
    return res;
  }

  std::string out;
  char buf[256];
  ssize_t n;
  while ((n = read(fd[0], buf, sizeof(buf))) > 0) {
    out.append(buf, n);
  }
  close(fd[0]);
  if ((WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 3) ||
      (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) == SIGKILL)) {
    // bad_alloc, or killed by the kernel oom-killer
    res.status = "oom";
    return res;
  }
  if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0 || out.empty()) {
    res.status = "error";
    return res;
  }
  std::stringstream ss(out);
  ss >> res.time_ms >> res.stats.max_states >> res.stats.mean_states >>
      res.stats.max_edges >> res.stats.mean_edges >>
      res.stats.n_translated >> res.stats.n_ignored;
  res.status = "ok";
  return res;
}

} // namespace

TEST(hybrid_similarity_families_scalabilityTests, families) {

  //clc::psilent = 1;
  clc::wsilent = 1;

  const size_t timeout_s = 2 * 60 * 60;

  // clang-format off
  std::vector<FamilyConfig> families = {
      {"..##1..",     "F2", "..##1..",    3, 1},
      {"until",       "F5", "until",      3, 0},
      {"eventually",  "F6", "eventually", 3, 0},
      {"next_len5",   "F2", "..##1..",    5, 1},
      {"next_len8",   "F2", "..##1..",    8, 1},
      {"next_delay3", "F2", "..##3..",    3, 3},
  };
  // shallower variants, tried only if the template does not complete at
  // 100x100, to find the largest tractable depth
  std::map<std::string, FamilyConfig> fallbacks = {
      {"next_len5",   {"next_len4",   "F2", "..##1..", 4, 1}},
      {"next_len8",   {"next_len6",   "F2", "..##1..", 6, 1}},
      {"next_delay3", {"next_delay2", "F2", "..##2..", 3, 2}},
  };
  // n_specs, n_vars
  std::vector<std::pair<size_t, size_t>> sizes = {
      {100, 100},
      {300, 300},
      {500, 100},
      {500, 500},
  };
  // clang-format on

  //dump to csv
  std::string dump_path = "hybrid_similarity_scalability_families.csv";

  //delete file if it exists
  if (std::filesystem::exists(dump_path)) {
    std::filesystem::remove(dump_path);
  }
  std::ofstream file(dump_path);
  file << "template,family,max_chain_length,delay,n_specs,n_vars,time_ms,"
          "max_states,mean_states,max_edges,mean_edges,status\n";
  file.close();

  auto dump = [&](const FamilyConfig &fc, size_t n_specs, size_t n_vars,
                  const RunResult &res) {
    std::ofstream file(dump_path, std::ios_base::app);
    file << fc.name << "," << fc.family << "," << fc.max_chain_length
         << "," << fc.delay << "," << n_specs << "," << n_vars << ",";
    if (res.status == "ok") {
      file << res.time_ms;
    }
    if (res.stats.n_translated > 0) {
      file << "," << res.stats.max_states << "," << res.stats.mean_states
           << "," << res.stats.max_edges << "," << res.stats.mean_edges;
    } else {
      file << ",,,,";
    }
    file << "," << res.status << "\n";
    file.close();
  };

  auto run = [&](const FamilyConfig &fc, size_t n_specs, size_t n_vars,
                 unsigned int seed) {
    std::cout << "Template: " << fc.name << " (" << fc.template_
              << ", max chain length " << fc.max_chain_length
              << ") Number of assertions: " << n_specs
              << " Number of variables: " << n_vars << " seed: " << seed
              << "\n";
    RunResult res = runWithTimeout(fc, n_specs, n_vars, seed, timeout_s);
    if (res.status != "ok") {
      // report the size of the automata that could not be compared
      RunResult sres =
          runWithTimeout(fc, n_specs, n_vars, seed, timeout_s, true);
      if (sres.status == "ok") {
        res.stats = sres.stats;
      }
    }
    std::cout << "Result: " << res.status << " time_ms: " << res.time_ms
              << " max_states: " << res.stats.max_states
              << " mean_states: " << res.stats.mean_states
              << " max_edges: " << res.stats.max_edges
              << " mean_edges: " << res.stats.mean_edges
              << " translated: " << res.stats.n_translated
              << " ignored: " << res.stats.n_ignored << "\n";
    dump(fc, n_specs, n_vars, res);
    return res;
  };

  // runs all sizes of a template, returns false if it does not complete at
  // the smallest size
  auto runAllSizes = [&](const FamilyConfig &fc, size_t f) {
    for (size_t s = 0; s < sizes.size(); s++) {
      auto [n_specs, n_vars] = sizes[s];
      // fixed seed per configuration
      unsigned int seed = 1000 * (f + 1) + s;
      RunResult res = run(fc, n_specs, n_vars, seed);
      if (res.status != "ok") {
        // skip the larger configurations of this template
        return s != 0;
      }
    }
    return true;
  };

  for (size_t f = 0; f < families.size(); f++) {
    const FamilyConfig &fc = families[f];
    if (!runAllSizes(fc, f) && fallbacks.count(fc.name)) {
      runAllSizes(fallbacks.at(fc.name), f);
    }
  }
}
