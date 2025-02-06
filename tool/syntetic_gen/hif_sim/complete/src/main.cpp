#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "cpptracer/tracer.hpp"
#include "muffin_dataTypes.hpp"
#include "test.hpp"

using Trace = std::vector<test::test_iostruct>;

// set the inputs to random values
void setRandomInputs(test::test_iostruct& in) {
  in.a_1_0 = rand() % 2;
  in.a_1_1 = rand() % 2;
  in.a_2_0 = rand() % 2;
  in.a_2_1 = rand() % 2;
  in.a_3_0 = rand() % 2;
  in.a_3_1 = rand() % 2;
  in.a_4_0 = rand() % 2;
  in.a_4_1 = rand() % 2;
  in.a_5_0 = rand() % 2;
  in.a_5_1 = rand() % 2;
  in.a_6_0 = rand() % 2;
  in.a_6_1 = rand() % 2;
  in.a_7_0 = rand() % 2;
  in.a_7_1 = rand() % 2;
  in.a_8_0 = rand() % 2;
  in.a_8_1 = rand() % 2;
  in.a_9_0 = rand() % 2;
  in.a_9_1 = rand() % 2;
  in.a_10_0 = rand() % 2;
  in.a_10_1 = rand() % 2;
}

// set the inputs from a trace sample
void setInputsFromTraceSample(test::test_iostruct& in,
                              const test::test_iostruct& dump) {
  in.a_1_0 = dump.a_1_0;
  in.a_1_1 = dump.a_1_1;
  in.a_2_0 = dump.a_2_0;
  in.a_2_1 = dump.a_2_1;
  in.a_3_0 = dump.a_3_0;
  in.a_3_1 = dump.a_3_1;
  in.a_4_0 = dump.a_4_0;
  in.a_4_1 = dump.a_4_1;
  in.a_5_0 = dump.a_5_0;
  in.a_5_1 = dump.a_5_1;
  in.a_6_0 = dump.a_6_0;
  in.a_6_1 = dump.a_6_1;
  in.a_7_0 = dump.a_7_0;
  in.a_7_1 = dump.a_7_1;
  in.a_8_0 = dump.a_8_0;
  in.a_8_1 = dump.a_8_1;
  in.a_9_0 = dump.a_9_0;
  in.a_9_1 = dump.a_9_1;
  in.a_10_0 = dump.a_10_0;
  in.a_10_1 = dump.a_10_1;
}

// returns true if two samples are equivalent
bool checkOutput(const test::test_iostruct& golden,
                 const test::test_iostruct& faulty) {
  if (golden.c_1_0 != faulty.c_1_0 || golden.c_1_1 != faulty.c_1_1 ||
      golden.c_2_0 != faulty.c_2_0 || golden.c_2_1 != faulty.c_2_1 ||
      golden.c_3_0 != faulty.c_3_0 || golden.c_3_1 != faulty.c_3_1 ||
      golden.c_4_0 != faulty.c_4_0 || golden.c_4_1 != faulty.c_4_1 ||
      golden.c_5_0 != faulty.c_5_0 || golden.c_5_1 != faulty.c_5_1 ||
      golden.c_6_0 != faulty.c_6_0 || golden.c_6_1 != faulty.c_6_1 ||
      golden.c_7_0 != faulty.c_7_0 || golden.c_7_1 != faulty.c_7_1 ||
      golden.c_8_0 != faulty.c_8_0 || golden.c_8_1 != faulty.c_8_1 ||
      golden.c_9_0 != faulty.c_9_0 || golden.c_9_1 != faulty.c_9_1 ||
      golden.c_10_0 != faulty.c_10_0 || golden.c_10_1 != faulty.c_10_1) {
    return 0;
  }
  return 1;
}

void printSample(test::test_iostruct& in) {
  printf("a_1_0: %d\n", in.a_1_0);
  printf("a_1_1: %d\n", in.a_1_1);
  printf("a_2_0: %d\n", in.a_2_0);
  printf("a_2_1: %d\n", in.a_2_1);
  printf("a_3_0: %d\n", in.a_3_0);
  printf("a_3_1: %d\n", in.a_3_1);
  printf("a_4_0: %d\n", in.a_4_0);
  printf("a_4_1: %d\n", in.a_4_1);
  printf("a_5_0: %d\n", in.a_5_0);
  printf("a_5_1: %d\n", in.a_5_1);
  printf("a_6_0: %d\n", in.a_6_0);
  printf("a_6_1: %d\n", in.a_6_1);
  printf("a_7_0: %d\n", in.a_7_0);
  printf("a_7_1: %d\n", in.a_7_1);
  printf("a_8_0: %d\n", in.a_8_0);
  printf("a_8_1: %d\n", in.a_8_1);
  printf("a_9_0: %d\n", in.a_9_0);
  printf("a_9_1: %d\n", in.a_9_1);
  printf("a_10_0: %d\n", in.a_10_0);
  printf("a_10_1: %d\n", in.a_10_1);
  printf("c_1_0: %d\n", in.c_1_0);
  printf("c_1_1: %d\n", in.c_1_1);
  printf("c_2_0: %d\n", in.c_2_0);
  printf("c_2_1: %d\n", in.c_2_1);
  printf("c_3_0: %d\n", in.c_3_0);
  printf("c_3_1: %d\n", in.c_3_1);
  printf("c_4_0: %d\n", in.c_4_0);
  printf("c_4_1: %d\n", in.c_4_1);
  printf("c_5_0: %d\n", in.c_5_0);
  printf("c_5_1: %d\n", in.c_5_1);
  printf("c_6_0: %d\n", in.c_6_0);
  printf("c_6_1: %d\n", in.c_6_1);
  printf("c_7_0: %d\n", in.c_7_0);
  printf("c_7_1: %d\n", in.c_7_1);
  printf("c_8_0: %d\n", in.c_8_0);
  printf("c_8_1: %d\n", in.c_8_1);
  printf("c_9_0: %d\n", in.c_9_0);
  printf("c_9_1: %d\n", in.c_9_1);
  printf("c_10_0: %d\n", in.c_10_0);
  printf("c_10_1: %d\n", in.c_10_1);
}

//we need to this to dump as a wire 1 bit long
static std::vector<bool> vcd_clock = {0};
static std::vector<bool> vcd_a_1_0 = {0};
static std::vector<bool> vcd_a_1_1 = {0};
static std::vector<bool> vcd_a_2_0 = {0};
static std::vector<bool> vcd_a_2_1 = {0};
static std::vector<bool> vcd_a_3_0 = {0};
static std::vector<bool> vcd_a_3_1 = {0};
static std::vector<bool> vcd_a_4_0 = {0};
static std::vector<bool> vcd_a_4_1 = {0};
static std::vector<bool> vcd_a_5_0 = {0};
static std::vector<bool> vcd_a_5_1 = {0};
static std::vector<bool> vcd_a_6_0 = {0};
static std::vector<bool> vcd_a_6_1 = {0};
static std::vector<bool> vcd_a_7_0 = {0};
static std::vector<bool> vcd_a_7_1 = {0};
static std::vector<bool> vcd_a_8_0 = {0};
static std::vector<bool> vcd_a_8_1 = {0};
static std::vector<bool> vcd_a_9_0 = {0};
static std::vector<bool> vcd_a_9_1 = {0};
static std::vector<bool> vcd_a_10_0 = {0};
static std::vector<bool> vcd_a_10_1 = {0};
static std::vector<bool> vcd_c_1_0 = {0};
static std::vector<bool> vcd_c_1_1 = {0};
static std::vector<bool> vcd_c_2_0 = {0};
static std::vector<bool> vcd_c_2_1 = {0};
static std::vector<bool> vcd_c_3_0 = {0};
static std::vector<bool> vcd_c_3_1 = {0};
static std::vector<bool> vcd_c_4_0 = {0};
static std::vector<bool> vcd_c_4_1 = {0};
static std::vector<bool> vcd_c_5_0 = {0};
static std::vector<bool> vcd_c_5_1 = {0};
static std::vector<bool> vcd_c_6_0 = {0};
static std::vector<bool> vcd_c_6_1 = {0};
static std::vector<bool> vcd_c_7_0 = {0};
static std::vector<bool> vcd_c_7_1 = {0};
static std::vector<bool> vcd_c_8_0 = {0};
static std::vector<bool> vcd_c_8_1 = {0};
static std::vector<bool> vcd_c_9_0 = {0};
static std::vector<bool> vcd_c_9_1 = {0};
static std::vector<bool> vcd_c_10_0 = {0};
static std::vector<bool> vcd_c_10_1 = {0};

static cpptracer::TimeScale timeStep(1, cpptracer::TimeUnit::NS);

cpptracer::Tracer initVCDTrace(const std::string& name) {
  cpptracer::Tracer tracer(name, timeStep, "test_bench");
  tracer.addScope("test_");

  tracer.addTrace(vcd_clock, "clock");
  tracer.addTrace(vcd_a_1_0, "a_1_0");
  tracer.addTrace(vcd_a_1_1, "a_1_1");
  tracer.addTrace(vcd_a_2_0, "a_2_0");
  tracer.addTrace(vcd_a_2_1, "a_2_1");
  tracer.addTrace(vcd_a_3_0, "a_3_0");
  tracer.addTrace(vcd_a_3_1, "a_3_1");
  tracer.addTrace(vcd_a_4_0, "a_4_0");
  tracer.addTrace(vcd_a_4_1, "a_4_1");
  tracer.addTrace(vcd_a_5_0, "a_5_0");
  tracer.addTrace(vcd_a_5_1, "a_5_1");
  tracer.addTrace(vcd_a_6_0, "a_6_0");
  tracer.addTrace(vcd_a_6_1, "a_6_1");
  tracer.addTrace(vcd_a_7_0, "a_7_0");
  tracer.addTrace(vcd_a_7_1, "a_7_1");
  tracer.addTrace(vcd_a_8_0, "a_8_0");
  tracer.addTrace(vcd_a_8_1, "a_8_1");
  tracer.addTrace(vcd_a_9_0, "a_9_0");
  tracer.addTrace(vcd_a_9_1, "a_9_1");
  tracer.addTrace(vcd_a_10_0, "a_10_0");
  tracer.addTrace(vcd_a_10_1, "a_10_1");
  tracer.addTrace(vcd_c_1_0, "c_1_0");
  tracer.addTrace(vcd_c_1_1, "c_1_1");
  tracer.addTrace(vcd_c_2_0, "c_2_0");
  tracer.addTrace(vcd_c_2_1, "c_2_1");
  tracer.addTrace(vcd_c_3_0, "c_3_0");
  tracer.addTrace(vcd_c_3_1, "c_3_1");
  tracer.addTrace(vcd_c_4_0, "c_4_0");
  tracer.addTrace(vcd_c_4_1, "c_4_1");
  tracer.addTrace(vcd_c_5_0, "c_5_0");
  tracer.addTrace(vcd_c_5_1, "c_5_1");
  tracer.addTrace(vcd_c_6_0, "c_6_0");
  tracer.addTrace(vcd_c_6_1, "c_6_1");
  tracer.addTrace(vcd_c_7_0, "c_7_0");
  tracer.addTrace(vcd_c_7_1, "c_7_1");
  tracer.addTrace(vcd_c_8_0, "c_8_0");
  tracer.addTrace(vcd_c_8_1, "c_8_1");
  tracer.addTrace(vcd_c_9_0, "c_9_0");
  tracer.addTrace(vcd_c_9_1, "c_9_1");
  tracer.addTrace(vcd_c_10_0, "c_10_0");
  tracer.addTrace(vcd_c_10_1, "c_10_1");

  tracer.closeScope();
  tracer.createTrace();
  return tracer;
}
void updateVCDVariables(const test::test_iostruct& in) {
  vcd_clock[0] = in.clock;
  vcd_a_1_0[0] = in.a_1_0;
  vcd_a_1_1[0] = in.a_1_1;
  vcd_a_2_0[0] = in.a_2_0;
  vcd_a_2_1[0] = in.a_2_1;
  vcd_a_3_0[0] = in.a_3_0;
  vcd_a_3_1[0] = in.a_3_1;
  vcd_a_4_0[0] = in.a_4_0;
  vcd_a_4_1[0] = in.a_4_1;
  vcd_a_5_0[0] = in.a_5_0;
  vcd_a_5_1[0] = in.a_5_1;
  vcd_a_6_0[0] = in.a_6_0;
  vcd_a_6_1[0] = in.a_6_1;
  vcd_a_7_0[0] = in.a_7_0;
  vcd_a_7_1[0] = in.a_7_1;
  vcd_a_8_0[0] = in.a_8_0;
  vcd_a_8_1[0] = in.a_8_1;
  vcd_a_9_0[0] = in.a_9_0;
  vcd_a_9_1[0] = in.a_9_1;
  vcd_a_10_0[0] = in.a_10_0;
  vcd_a_10_1[0] = in.a_10_1;
  vcd_c_1_0[0] = in.c_1_0;
  vcd_c_1_1[0] = in.c_1_1;
  vcd_c_2_0[0] = in.c_2_0;
  vcd_c_2_1[0] = in.c_2_1;
  vcd_c_3_0[0] = in.c_3_0;
  vcd_c_3_1[0] = in.c_3_1;
  vcd_c_4_0[0] = in.c_4_0;
  vcd_c_4_1[0] = in.c_4_1;
  vcd_c_5_0[0] = in.c_5_0;
  vcd_c_5_1[0] = in.c_5_1;
  vcd_c_6_0[0] = in.c_6_0;
  vcd_c_6_1[0] = in.c_6_1;
  vcd_c_7_0[0] = in.c_7_0;
  vcd_c_7_1[0] = in.c_7_1;
  vcd_c_8_0[0] = in.c_8_0;
  vcd_c_8_1[0] = in.c_8_1;
  vcd_c_9_0[0] = in.c_9_0;
  vcd_c_9_1[0] = in.c_9_1;
  vcd_c_10_0[0] = in.c_10_0;
  vcd_c_10_1[0] = in.c_10_1;
}

bool areEquivalent(const Trace& t1, const Trace& t2) {
  if (t1.size() != t2.size()) {
    return false;
  }
  for (size_t i = 0; i < t1.size(); ++i) {
    if (t1[i].a_1_0 != t2[i].a_1_0 || t1[i].a_1_1 != t2[i].a_1_1 ||
        t1[i].a_2_0 != t2[i].a_2_0 || t1[i].a_2_1 != t2[i].a_2_1 ||
        t1[i].a_3_0 != t2[i].a_3_0 || t1[i].a_3_1 != t2[i].a_3_1 ||
        t1[i].a_4_0 != t2[i].a_4_0 || t1[i].a_4_1 != t2[i].a_4_1 ||
        t1[i].a_5_0 != t2[i].a_5_0 || t1[i].a_5_1 != t2[i].a_5_1 ||
        t1[i].a_6_0 != t2[i].a_6_0 || t1[i].a_6_1 != t2[i].a_6_1 ||
        t1[i].a_7_0 != t2[i].a_7_0 || t1[i].a_7_1 != t2[i].a_7_1 ||
        t1[i].a_8_0 != t2[i].a_8_0 || t1[i].a_8_1 != t2[i].a_8_1 ||
        t1[i].a_9_0 != t2[i].a_9_0 || t1[i].a_9_1 != t2[i].a_9_1 ||
        t1[i].a_10_0 != t2[i].a_10_0 || t1[i].a_10_1 != t2[i].a_10_1 ||
        t1[i].c_1_0 != t2[i].c_1_0 || t1[i].c_1_1 != t2[i].c_1_1 ||
        t1[i].c_2_0 != t2[i].c_2_0 || t1[i].c_2_1 != t2[i].c_2_1 ||
        t1[i].c_3_0 != t2[i].c_3_0 || t1[i].c_3_1 != t2[i].c_3_1 ||
        t1[i].c_4_0 != t2[i].c_4_0 || t1[i].c_4_1 != t2[i].c_4_1 ||
        t1[i].c_5_0 != t2[i].c_5_0 || t1[i].c_5_1 != t2[i].c_5_1 ||
        t1[i].c_6_0 != t2[i].c_6_0 || t1[i].c_6_1 != t2[i].c_6_1 ||
        t1[i].c_7_0 != t2[i].c_7_0 || t1[i].c_7_1 != t2[i].c_7_1 ||
        t1[i].c_8_0 != t2[i].c_8_0 || t1[i].c_8_1 != t2[i].c_8_1 ||
        t1[i].c_9_0 != t2[i].c_9_0 || t1[i].c_9_1 != t2[i].c_9_1 ||
        t1[i].c_10_0 != t2[i].c_10_0 || t1[i].c_10_1 != t2[i].c_10_1) {
      return false;
    }
  }
  return true;
}

int main() {
  muffin::stuck_at = 0;
  muffin::hif_global_instance_counter = 0;

  // number of faulted instances created with muffin, 0 is the original design
  // without faults
  muffin::instance_number = 0;

  // faults
  int32_t cycles_number = 0;
  size_t traceLength = 1000 * 2;  // 1000 positive and 1000 negative edges

  bool clock_0 = 0;

  printf("Simulate golden\n");

  test test_instance;
  test_instance.initialize();

  Trace golden_trace;

  // in case of a rst
  test::test_iostruct in_rst_on;
  in_rst_on.clock = clock_0;
  test_instance.simulate(&in_rst_on, cycles_number);

  srand(0);

  test::test_iostruct in;

  for (size_t k = 0; k < traceLength; ++k) {
    clock_0 = !clock_0;

    in.clock = clock_0;

    // in
    if (!clock_0) {
      setRandomInputs(in);
    }

    test_instance.simulate(&in, cycles_number);

    // out
    golden_trace.push_back(in);
    // printSample(in);
  }

  // Simulate the design with faults (instance 1), we need to reuse the same input trace
  muffin::instance_number = 1;
  size_t faultObserved = 0;
  std::vector<size_t> uncoveredFaults;
  std::vector<Trace> faultyTraces;
  std::cout << "Number of faults: " << test_instance.hif_fault_node.number
            << "\n";

  for (muffin::fault_number = 0;
       muffin::fault_number < test_instance.hif_fault_node.number;
       ++muffin::fault_number) {
    Trace faulty_trace;
    muffin::hif_global_instance_counter = 0;
    printf("Simulating fault number '%ld:%ld'\n", muffin::instance_number,
           muffin::fault_number);

    clock_0 = 0;
    test::test_iostruct in_rst_on;
    in_rst_on.clock = clock_0;
    test_instance.simulate(&in_rst_on, cycles_number);
    test test_instance;
    test_instance.initialize();

    test::test_iostruct in;
    bool faultFound = 0;
    for (size_t k = 0; k < traceLength; ++k) {
      clock_0 = !clock_0;

      in.clock = clock_0;
      // in
      if (!clock_0) {
        setInputsFromTraceSample(in, golden_trace[k]);
      }
      test_instance.simulate(&in, cycles_number);

      faulty_trace.push_back(in);

      // get the output
      if (clock_0) {
        if (!checkOutput(golden_trace[k], in)) {
          faultFound = 1;
        }
      }
    }

    if (faultFound) {
      faultyTraces.push_back(faulty_trace);
      ++faultObserved;
    } else {
      uncoveredFaults.push_back(muffin::fault_number);
    }
  }

  // remove redundant faulty traces
  for (auto it = faultyTraces.begin(); it != faultyTraces.end(); ++it) {
    for (auto it2 = it + 1; it2 != faultyTraces.end(); ++it2) {
      if (areEquivalent(*it, *it2)) {
        faultyTraces.erase(it2);
        --it2;
      }
    }
  }

  printf("Faults observed: %ld\n", faultObserved);
  for (auto fault : uncoveredFaults) {
    printf("Fault n %ld not observed\n", fault);
  }
  printf("Unique faulty traces: %ld\n", faultyTraces.size());

  //-------------------------------------------------------------------------
  // Dump traces to VCD
  // make traces directory if it does not exist
  if (!std::filesystem::exists("traces")) {
    std::filesystem::create_directory("traces");
  }

  auto tracer = initVCDTrace("traces/golden.vcd");
  double time = 0;
  for (auto& sample : golden_trace) {
    updateVCDVariables(sample);
    tracer.updateTrace(time);
    time += timeStep;
  }
  tracer.closeTrace();

  for (size_t i = 0; i < faultyTraces.size(); ++i) {
    time = 0;
    auto tracer = initVCDTrace("traces/fault_" + std::to_string(i) + ".vcd");
    for (auto& sample : faultyTraces[i]) {
      updateVCDVariables(sample);
      tracer.updateTrace(time);
      time += timeStep;
    }
    tracer.closeTrace();
  }

  return 0;
}
