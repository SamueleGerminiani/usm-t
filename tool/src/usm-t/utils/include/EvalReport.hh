
#pragma once
#include "message.hh"
#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace usmt {

class EvalReport {
public:
  EvalReport(std::string with_strategy)
      : _with_strategy(with_strategy) {}
  ~EvalReport() {}
  virtual void dumpTo(const std::string &pathToDir) = 0;
  virtual std::string to_string() = 0;
  std::string _with_strategy;
};

class FaultCoverageReport : public EvalReport {

public:
  FaultCoverageReport() : EvalReport("fault_coverage") {}

  virtual ~FaultCoverageReport() = default;
  virtual std::string to_string() override {
    std::stringstream ss;
    ss << "Fault Coverage Report\n";

    ss << "Fault coverage: ";
    ss << "\t" << _totFaults - _uncoveredFaults.size() << "/"
       << _totFaults << " (" << fault_coverage * 100.f << "%)\n";

    if (_minCoveringAssertions.size() > 0) {
      ss << "Min covering set size: " << _minCoveringAssertions.size()
         << "\n";
      ss << "-------Min Covering Assertions----------\n";
      for (const auto &assertion : _minCoveringAssertions) {
        ss << "\t" << assertion << "\n";
      }
    }

    if (_uncoveredFaults.size() > 0) {
      ss << "---Uncovered Faults-------------------\n";
      for (const auto &fault : _uncoveredFaults) {
        ss << "\t" << fault << "\n";
      }
    }
    ss << "----------------------------------------\n";

    return ss.str();
  }

  virtual void dumpTo(const std::string &pathToDir) override {
    messageInfo("Dumping Fault Coverage Report to: " + pathToDir);
    std::ofstream out(pathToDir + "/fault_coverage_report.csv");
    out << "Fault covered, Covered %,  Min Covering Size, Min "
           "Covering Set\n";
    out << _totFaults - _uncoveredFaults.size() << ", "
        << fault_coverage * 100.f << ", "
        << _minCoveringAssertions.size() << ", ";
    for (const auto &assertion : _minCoveringAssertions) {
      out << assertion << " ";
    }
    out << "\n";
  }

  double fault_coverage = 0.f;
  std::vector<std::string> _uncoveredFaults;
  std::vector<std::string> _minCoveringAssertions;
  size_t _totFaults;
};

class SemanticEquivalenceReport : public EvalReport {
public:
  SemanticEquivalenceReport() : EvalReport("semantic_equivalence") {}
  ~SemanticEquivalenceReport() = default;

  virtual std::string to_string() override {
    std::stringstream ss;
    size_t totExpected = _expectedToCoveredWith.size() +
                         _expectedToSimilar.size() +
                         _uncovered.size();
    ss << "Semantic equivalence report\n";
    ss << "Semantically Covered " << _expectedToCoveredWith.size()
       << "/" << totExpected << "\n";

    if (_expectedToCoveredWith.size() != totExpected) {
      ss << "SSimilar " << _expectedToSimilar.size() << "/"
         << totExpected << "\n";
      ss << "--------Covered-"
         << "------------------\n";
    }

    for (const auto &pair : _expectedToCoveredWith) {
      ss << pair.first << "\n";
      ss << "--->  \t\t" << pair.second << "\n";
    }
    if (_expectedToCoveredWith.size() != totExpected) {
      ss << "--------Semantically Similar "
         << "------------------\n";

      for (const auto &pair : _expectedToSimilar) {
        ss << pair.first << "\n";
        for (const auto &similar : pair.second) {
          ss << "\t\t\t->  " << similar << "\n";
        }
      }
    }

    ss << "Final score: " << _final_score << "\n";
    ss << "Noise: " << _noise << "\n";

    return ss.str();
  }

  virtual void dumpTo(const std::string &pathToDir) override {
    messageInfo("Dumping semantic equivalence report to: " +
                pathToDir);
    std::ofstream out(pathToDir + "/semantic_equivalence_report.csv");
    out << "Expected, CoveredWith, Similar\n";
    for (const auto &pair : _expectedToCoveredWith) {
      out << pair.first << ", " << pair.second << ", None \n";
    }

    for (const auto &pair : _expectedToSimilar) {
      for (const auto &similar : pair.second) {
        out << pair.first << ", None , " << similar << "\n";
      }
    }
  }

  std::unordered_map<std::string, std::unordered_set<std::string>>
      _expectedToSimilar;
  std::unordered_map<std::string, std::string> _expectedToCoveredWith;
  std::vector<std::string> _uncovered;
  double _final_score = 0.f;
  double _noise = 0.f;
};

class EditDistanceReport : public EvalReport {
public:
  EditDistanceReport() : EvalReport("edit_distance") {}
  ~EditDistanceReport() = default;

  virtual std::string to_string() override {
    std::stringstream ss;
    ss << "Edit distance Report\n";
    ss << "----------------Edit similarity------------------\n";
    for (const auto &pair : _expectedToClosest) {
      ss << pair.first << "\n\t\t--> " << pair.second.first << " : "
         << pair.second.second << "\n";
    }
    ss << "Final score: " << _final_score << "\n";
    ss << "Noise: " << _noise << "\n";

    return ss.str();
  }

  virtual void dumpTo(const std::string &pathToDir) override {
    messageInfo("Dumping edit distance report to: " + pathToDir);
    std::ofstream out(pathToDir + "/edit_distance_report.csv");
    out << "Expected, BestCoveredWith, Similarity\n";
    for (const auto &[expected, mostSimilar] : _expectedToClosest) {
      auto &[mostSimilar_assertion, similarity] =
          _expectedToClosest[expected];
      out << expected << ", " << mostSimilar_assertion << ", "
          << similarity << "\n";
    }
  }

  std::unordered_map<std::string, std::pair<std::string, double>>
      _expectedToClosest;
  double _final_score = 0.f;
  double _noise = 0.f;
};

class SyntacticSimilarityReport : public EvalReport {
public:
  SyntacticSimilarityReport() : EvalReport("syntactic_similarity") {}
  ~SyntacticSimilarityReport() = default;

  virtual std::string to_string() override {
    std::stringstream ss;
    ss << "Syntactic similarity Report\n";
    ss << "----------------Syntactic similarity------------------\n";
    for (const auto &pair : _expectedToClosest) {
      ss << pair.first << "\n\t\t--> " << pair.second.first << " : "
         << pair.second.second << "\n";
    }
    ss << "Final score: " << _final_score << "\n";
    ss << "Noise: " << _noise << "\n";

    return ss.str();
  }

  virtual void dumpTo(const std::string &pathToDir) override {
    messageInfo("Dumping syntactic similarty report to: " +
                pathToDir);
    std::ofstream out(pathToDir + "/syntactic_similarity.csv");
    out << "Expected, BestCoveredWith, Similarity\n";
    for (const auto &[expected, mostSimilar] : _expectedToClosest) {
      auto &[mostSimilar_assertion, similarity] =
          _expectedToClosest[expected];
      out << expected << ", " << mostSimilar_assertion << ", "
          << similarity << "\n";
    }
  }

  std::unordered_map<std::string, std::pair<std::string, double>>
      _expectedToClosest;
  double _final_score = 0.f;
  double _noise = 0.f;
};

class TemporalReport : public EvalReport {
public:
  TemporalReport() : EvalReport("time_to_mine") {}
  ~TemporalReport() = default;

  virtual std::string to_string() override {
    std::stringstream ss;
    ss << "Temporal Report\n";
    ss << "Time: " << (double)_timeMS / 1000.f << " ms\n";
    return ss.str();
  }

  virtual void dumpTo(const std::string &pathToDir) override {
    messageInfo("Dumping Temporal Report to: " + pathToDir);
    std::ofstream out(pathToDir + "/temporal_report.csv");
    out << "Time\n";
    out << (double)_timeMS / 1000.f << "\n";
  }

  size_t _timeMS = 0;
};

using EvalReportPtr = std::shared_ptr<EvalReport>;
using FaultCoverageReportPtr = std::shared_ptr<FaultCoverageReport>;
using SemanticEquivalenceReportPtr =
    std::shared_ptr<SemanticEquivalenceReport>;
using EditDistanceReportPtr = std::shared_ptr<EditDistanceReport>;
using SyntacticSimilarityReportPtr =
    std::shared_ptr<SyntacticSimilarityReport>;
using TemporalReportPtr = std::shared_ptr<TemporalReport>;

} // namespace usmt
