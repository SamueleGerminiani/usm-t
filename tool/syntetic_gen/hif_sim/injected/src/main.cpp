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


void setRandomInputs(test::test_iostruct& in) {
	in.b_1 = rand() % 2;
	in.b_3 = rand() % 2;
	in.a_3 = rand() % 2;
	in.a_1 = rand() % 2;
	in.a_2 = rand() % 2;
	in.b_2 = rand() % 2;
}
void setInputsFromTraceSample(test::test_iostruct& in, const test::test_iostruct& dump) {
	in.b_1 = dump.b_1;
	in.b_3 = dump.b_3;
	in.a_3 = dump.a_3;
	in.a_1 = dump.a_1;
	in.a_2 = dump.a_2;
	in.b_2 = dump.b_2;
}
bool checkOutput(const test::test_iostruct& golden, const test::test_iostruct& faulty) {
 if (golden.c_1 != faulty.c_1 || golden.c_2 != faulty.c_2 || golden.c_3 != faulty.c_3 ) {
 return 0;
}
 return 1;
}
void printSample(test::test_iostruct& in) {printf("b_1: %d\n", in.b_1);
printf("b_3: %d\n", in.b_3);
printf("a_3: %d\n", in.a_3);
printf("a_1: %d\n", in.a_1);
printf("a_2: %d\n", in.a_2);
printf("b_2: %d\n", in.b_2);
}
static std::vector<bool> vcd_clock = {0};
 static std::vector<bool> vcd_b_1 = {0};
static std::vector<bool> vcd_b_3 = {0};
static std::vector<bool> vcd_a_3 = {0};
static std::vector<bool> vcd_a_1 = {0};
static std::vector<bool> vcd_a_2 = {0};
static std::vector<bool> vcd_b_2 = {0};
static std::vector<bool> vcd_c_1 = {0};
static std::vector<bool> vcd_c_2 = {0};
static std::vector<bool> vcd_c_3 = {0};

static cpptracer::TimeScale timeStep(1, cpptracer::TimeUnit::NS);

cpptracer::Tracer initVCDTrace(const std::string& name) {
 cpptracer::Tracer tracer(name, timeStep,"test_bench");
 tracer.addScope("test_");

tracer.addTrace(vcd_clock, "clock");
tracer.addTrace(vcd_b_1, "b_1");
tracer.addTrace(vcd_b_3, "b_3");
tracer.addTrace(vcd_a_3, "a_3");
tracer.addTrace(vcd_a_1, "a_1");
tracer.addTrace(vcd_a_2, "a_2");
tracer.addTrace(vcd_b_2, "b_2");
tracer.addTrace(vcd_c_1, "c_1");
tracer.addTrace(vcd_c_2, "c_2");
tracer.addTrace(vcd_c_3, "c_3");
tracer.closeScope();
 tracer.createTrace();
 return tracer;
}
void updateVCDVariables(const test::test_iostruct& in) {
 vcd_clock[0] = in.clock;
vcd_b_1[0] = in.b_1;
vcd_b_3[0] = in.b_3;
vcd_a_3[0] = in.a_3;
vcd_a_1[0] = in.a_1;
vcd_a_2[0] = in.a_2;
vcd_b_2[0] = in.b_2;
vcd_c_1[0] = in.c_1;
vcd_c_2[0] = in.c_2;
vcd_c_3[0] = in.c_3;
}
bool areEquivalent(const Trace& t1, const Trace& t2) {
if (t1.size() != t2.size()) {
 return false;
}
for (size_t i = 0; i < t1.size(); ++i) {
if (t1[i].b_1 != t2[i].b_1 || t1[i].b_3 != t2[i].b_3 || t1[i].a_3 != t2[i].a_3 || t1[i].a_1 != t2[i].a_1 || t1[i].a_2 != t2[i].a_2 || t1[i].b_2 != t2[i].b_2 || t1[i].c_1 != t2[i].c_1 || t1[i].c_2 != t2[i].c_2 || t1[i].c_3 != t2[i].c_3 ) {
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

          printf("Simulate golden \n");

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
          // Simulate the design with faults (instance 1 to 4), 1 is test with 0 faults
          // input trace
          size_t faultObserved = 0;
          std::vector<std::pair<size_t, size_t>> uncoveredFaults;
          std::vector<Trace> faultyTraces;
          std::cout << "Number of faults: " << test_instance.hif_fault_node.number
                    << "\n";

          std::cout << "N instances: " << muffin::hif_global_instance_counter << "\n";
          std::cout << "N faults test: " << test_instance.hif_fault_node.number << "\n";
          std::cout << "N faults spec0: "
                    << test_instance.spec_sbm0.hif_fault_node.number << "\n";
          std::cout << "N faults spec1: "
                    << test_instance.spec_sbm1.hif_fault_node.number << "\n";
          std::cout << "N faults spec2: "
                    << test_instance.spec_sbm2.hif_fault_node.number << "\n";

          std::vector<size_t> instanceToNumberOfFaults;
          instanceToNumberOfFaults.push_back(0);
instanceToNumberOfFaults.push_back(test_instance.hif_fault_node.number);
instanceToNumberOfFaults.push_back(test_instance.spec_sbm0.hif_fault_node.number);
instanceToNumberOfFaults.push_back(test_instance.spec_sbm1.hif_fault_node.number);
instanceToNumberOfFaults.push_back(test_instance.spec_sbm2.hif_fault_node.number);

          for (size_t curr_instance_number = 1;
               curr_instance_number <= instanceToNumberOfFaults.size() - 1;
               ++curr_instance_number) {
            muffin::instance_number = curr_instance_number;
            size_t nFaults = instanceToNumberOfFaults[curr_instance_number];
          
            for (muffin::fault_number = 0; muffin::fault_number < nFaults;
                 ++muffin::fault_number) {
              Trace faulty_trace;
              printf("Simulating fault number '%ld:%ld'\n", muffin::instance_number,
                     muffin::fault_number);
          
              clock_0 = 0;
              test::test_iostruct in_rst_on;
              in_rst_on.clock = clock_0;
              test_instance.simulate(&in_rst_on, cycles_number);
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
                uncoveredFaults.emplace_back(muffin::fault_number,
                                             muffin::instance_number);
              }
          
            }  // end for fault_number
          }    // end for instance_number
    

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