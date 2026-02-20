#include <algorithm>
#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <stddef.h>
#include <string>
#include <vector>

#include "CSVtraceReader.hh"
#include "Float.hh"
#include "TemplateImplication.hh"
#include "Trace.hh"
#include "TraceReader.hh"
#include "VCDtraceReader.hh"
#include "formula/atom/Atom.hh"
#include "formula/atom/Variable.hh"
#include "globals.hh"
#include "misc.hh"
#include "temporalParsingUtils.hh"
#include "gtest/gtest_pred_impl.h"

using namespace harm;
using namespace expression;
using namespace hparser;

class TraceTest : public ::testing::Test {
protected:
  void TearDown() override { delete tr; }

public:
  TraceReader *tr = nullptr;
  const TracePtr &trace = nullptr;
};
std::vector<std::string> readLines(const std::string &fp) {
  std::vector<std::string> lines;
  std::ifstream file(fp);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + fp);
  }
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  file.close();
  return lines;
}

/////test single csv trace
//TEST_F(TraceTest, GarbageInParser) {
//
//  tr = new CSVtraceReader("../tests/input/BasicTypes.csv");
//  const TracePtr &trace = tr->readTrace();
//  std::vector<std::string> specs =
//      readLines("../tests/input/GarbageSpecifications.txt");
//
//  std::string fp = "../tests/input/GarbageSpecifications.txt";
//  for (const auto &s : specs) {
//    if (s == "") {
//      continue; // skip empty lines
//    }
//    std::cout << "Parsing specification: " << s << "\n";
//  }
//}
