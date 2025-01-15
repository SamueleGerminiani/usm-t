
#pragma once
#include "expUtils/Z3ExpWrapper.hh"
#include "formula/atom/Atom.hh"
#include "visitors/ExpToZ3Visitor.hh"

#include "message.hh"

#include <bitset>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace expression {
enum class ExpType;
}

namespace z3 {
struct VarValue {
  VarValue() {}
  VarValue(uint64_t value) : uVal(value) {}
  VarValue(int64_t value) : sVal(value) {}
  VarValue(int64_t value, std::string name)
      : sVal(value), namedValue(name) {}
  VarValue(bool value) : bVal(value) {}
  VarValue(double value) : dVal(value) {}
  VarValue(float value) : dVal(value) {}
  uint64_t uVal;
  int64_t sVal;
  bool bVal;
  double dVal;
  std::string namedValue = "";
};

using VarName = std::string;
using VarType = std::pair<expression::ExpType, size_t>;
using TypeValue = std::pair<expression::ExpType, VarValue>;
using Assignment = std::pair<VarName, TypeValue>;
using TestCase = std::vector<Assignment>;
std::ostream &operator<<(std::ostream &o, const TestCase &tc);

union intToFloat {
  uint32_t i;
  float f;
};
union intToDouble {
  uint64_t i;
  double d;
};

bool operator==(TypeValue e1, TypeValue e2);
std::ostream &operator<<(std::ostream &os, const TypeValue e1);

class Z3TestCaseGenerator {
public:
  Z3TestCaseGenerator() = default;

  std::vector<TestCase>
  generateTestCase(expression::PropositionPtr &p, size_t n = 1,
                   const std::vector<TestCase> &alreadyGen =
                       std::vector<TestCase>());

  std::vector<expression::Z3ExpWrapper>
  propToz3Expr(std::vector<expression::Proposition *> &ps);
  std::vector<expression::Z3ExpWrapper> propToz3Expr(
      std::vector<std::shared_ptr<expression::Proposition>> &ps);
  z3::context *getContext();

private:
  z3::expr negateTestCases(
      const std::vector<TestCase> &tcs, context &c,
      const std::vector<std::pair<VarName, VarType>> &contained_vars);
  //expression::ExpToZ3Visitor _converter;
};
inline std::string valueToString(Assignment &ass) {
  if (ass.second.second.namedValue != "") {
    return ass.second.second.namedValue;
  }
  std::stringstream ss;
  switch (ass.second.first) {

    return std::to_string(ass.second.second.bVal);
    break;
  case expression::ExpType::UInt:
    return std::to_string(ass.second.second.uVal);
    break;
  case expression::ExpType::SInt:
    return std::to_string(ass.second.second.sVal);
    break;
  case expression::ExpType::Float:
    ss << std::setprecision(12) << ass.second.second.dVal;
    return ss.str();
    break;
  default:
    return "Uknown";
  }
  return "Uknown";
}

} // namespace z3
