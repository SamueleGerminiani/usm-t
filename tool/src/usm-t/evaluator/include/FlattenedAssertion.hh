#pragma once
#include "Assertion.hh"
#include <string>
#include <unordered_map>

namespace usmt {
struct FlattenedAssertion {
  FlattenedAssertion(const harm::AssertionPtr &original,
                     const std::string &flattened_str)
      : original(original), flattened_str(flattened_str) {}
  FlattenedAssertion() = default;
  harm::AssertionPtr original;
  std::string flattened_str;
};

std::unordered_map<std::string, std::vector<FlattenedAssertion>>
getFlattenedAssertions(
    const std::vector<harm::AssertionPtr> &expected_assertions,
    const std::vector<harm::AssertionPtr> &mined_assertions,
    std::unordered_map<std::string, std::string> &targetToRemap);
} // namespace usmt
