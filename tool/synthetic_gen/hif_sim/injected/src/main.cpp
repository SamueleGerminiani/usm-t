#include <stdlib.h>
#include <algorithm>
 #include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "cpptracer/tracer.hpp"
#include "muffin_dataTypes.hpp"
#include "top_module.hpp"

using Trace = std::vector<top_module::top_module_iostruct>;


void setRandomInputs(top_module::top_module_iostruct& in) {
	in.b_0 = rand() % 2;
	in.b_4 = rand() % 2;
	in.a_0 = rand() % 2;
	in.b_5 = rand() % 2;
	in.a_2 = rand() % 2;
	in.a_3 = rand() % 2;
	in.a_1 = rand() % 2;
	in.b_3 = rand() % 2;
	in.b_1 = rand() % 2;
	in.a_5 = rand() % 2;
	in.b_2 = rand() % 2;
	in.a_4 = rand() % 2;
}
void setInputsFromTraceSample(top_module::top_module_iostruct& in, const top_module::top_module_iostruct& dump) {
	in.b_0 = dump.b_0;
	in.b_4 = dump.b_4;
	in.a_0 = dump.a_0;
	in.b_5 = dump.b_5;
	in.a_2 = dump.a_2;
	in.a_3 = dump.a_3;
	in.a_1 = dump.a_1;
	in.b_3 = dump.b_3;
	in.b_1 = dump.b_1;
	in.a_5 = dump.a_5;
	in.b_2 = dump.b_2;
	in.a_4 = dump.a_4;
}
bool checkOutput(const top_module::top_module_iostruct& golden, const top_module::top_module_iostruct& faulty) {
 if (golden.c_2 != faulty.c_2 || golden.c_0 != faulty.c_0 || golden.c_5 != faulty.c_5 || golden.c_4 != faulty.c_4 || golden.c_1 != faulty.c_1 || golden.c_3 != faulty.c_3 ) {
 return 0;
}
 return 1;
}
void printSample(top_module::top_module_iostruct& in) {printf("b_0: %d\n", in.b_0);
printf("b_4: %d\n", in.b_4);
printf("a_0: %d\n", in.a_0);
printf("b_5: %d\n", in.b_5);
printf("a_2: %d\n", in.a_2);
printf("a_3: %d\n", in.a_3);
printf("a_1: %d\n", in.a_1);
printf("b_3: %d\n", in.b_3);
printf("b_1: %d\n", in.b_1);
printf("a_5: %d\n", in.a_5);
printf("b_2: %d\n", in.b_2);
printf("a_4: %d\n", in.a_4);
}
static std::vector<bool> vcd_clock = {0};
 static std::vector<bool> vcd_b_0 = {0};
static std::vector<bool> vcd_b_4 = {0};
static std::vector<bool> vcd_a_0 = {0};
static std::vector<bool> vcd_b_5 = {0};
static std::vector<bool> vcd_a_2 = {0};
static std::vector<bool> vcd_a_3 = {0};
static std::vector<bool> vcd_a_1 = {0};
static std::vector<bool> vcd_b_3 = {0};
static std::vector<bool> vcd_b_1 = {0};
static std::vector<bool> vcd_a_5 = {0};
static std::vector<bool> vcd_b_2 = {0};
static std::vector<bool> vcd_a_4 = {0};
static std::vector<bool> vcd_c_2 = {0};
static std::vector<bool> vcd_c_0 = {0};
static std::vector<bool> vcd_c_5 = {0};
static std::vector<bool> vcd_c_4 = {0};
static std::vector<bool> vcd_c_1 = {0};
static std::vector<bool> vcd_c_3 = {0};

static cpptracer::TimeScale timeStep(1, cpptracer::TimeUnit::NS);

cpptracer::Tracer initVCDTrace(const std::string& name) {
 cpptracer::Tracer tracer(name, timeStep,"top_module_bench");
 tracer.addScope("top_module_");

tracer.addTrace(vcd_clock, "clock");
tracer.addTrace(vcd_b_0, "b_0");
tracer.addTrace(vcd_b_4, "b_4");
tracer.addTrace(vcd_a_0, "a_0");
tracer.addTrace(vcd_b_5, "b_5");
tracer.addTrace(vcd_a_2, "a_2");
tracer.addTrace(vcd_a_3, "a_3");
tracer.addTrace(vcd_a_1, "a_1");
tracer.addTrace(vcd_b_3, "b_3");
tracer.addTrace(vcd_b_1, "b_1");
tracer.addTrace(vcd_a_5, "a_5");
tracer.addTrace(vcd_b_2, "b_2");
tracer.addTrace(vcd_a_4, "a_4");
tracer.addTrace(vcd_c_2, "c_2");
tracer.addTrace(vcd_c_0, "c_0");
tracer.addTrace(vcd_c_5, "c_5");
tracer.addTrace(vcd_c_4, "c_4");
tracer.addTrace(vcd_c_1, "c_1");
tracer.addTrace(vcd_c_3, "c_3");
tracer.closeScope();
 tracer.createTrace();
 return tracer;
}
void updateVCDVariables(const top_module::top_module_iostruct& in) {
 vcd_clock[0] = in.clock;
vcd_b_0[0] = in.b_0;
vcd_b_4[0] = in.b_4;
vcd_a_0[0] = in.a_0;
vcd_b_5[0] = in.b_5;
vcd_a_2[0] = in.a_2;
vcd_a_3[0] = in.a_3;
vcd_a_1[0] = in.a_1;
vcd_b_3[0] = in.b_3;
vcd_b_1[0] = in.b_1;
vcd_a_5[0] = in.a_5;
vcd_b_2[0] = in.b_2;
vcd_a_4[0] = in.a_4;
vcd_c_2[0] = in.c_2;
vcd_c_0[0] = in.c_0;
vcd_c_5[0] = in.c_5;
vcd_c_4[0] = in.c_4;
vcd_c_1[0] = in.c_1;
vcd_c_3[0] = in.c_3;
}
bool areEquivalent(const Trace& t1, const Trace& t2) {
if (t1.size() != t2.size()) {
 return false;
}
for (size_t i = 0; i < t1.size(); ++i) {
if (t1[i].b_0 != t2[i].b_0 || t1[i].b_4 != t2[i].b_4 || t1[i].a_0 != t2[i].a_0 || t1[i].b_5 != t2[i].b_5 || t1[i].a_2 != t2[i].a_2 || t1[i].a_3 != t2[i].a_3 || t1[i].a_1 != t2[i].a_1 || t1[i].b_3 != t2[i].b_3 || t1[i].b_1 != t2[i].b_1 || t1[i].a_5 != t2[i].a_5 || t1[i].b_2 != t2[i].b_2 || t1[i].a_4 != t2[i].a_4 || t1[i].c_2 != t2[i].c_2 || t1[i].c_0 != t2[i].c_0 || t1[i].c_5 != t2[i].c_5 || t1[i].c_4 != t2[i].c_4 || t1[i].c_1 != t2[i].c_1 || t1[i].c_3 != t2[i].c_3 ) {
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

          top_module top_module_instance;
          top_module_instance.initialize();

          Trace golden_trace;

          // in case of a rst
          top_module::top_module_iostruct in_rst_on;
          in_rst_on.clock = clock_0;
          top_module_instance.simulate(&in_rst_on, cycles_number);

          srand(0);

          top_module::top_module_iostruct in;

          for (size_t k = 0; k < traceLength; ++k) {
            clock_0 = !clock_0;

            in.clock = clock_0;

            // in
            if (!clock_0) {
              setRandomInputs(in);
            }

            top_module_instance.simulate(&in, cycles_number);

            // out
            golden_trace.push_back(in);
            // printSample(in);
          }
          // Simulate the design with faults (instance 1 to 4), 1 is top_module with 0 faults
          // input trace
          size_t faultObserved = 0;
          std::vector<std::pair<size_t, size_t>> uncoveredFaults;
          std::vector<Trace> faultyTraces;
          std::cout << "Number of faults: " << top_module_instance.hif_fault_node.number
                    << "\n";

          std::cout << "N instances: " << muffin::hif_global_instance_counter << "\n";
          std::cout << "N faults top_module: " << top_module_instance.hif_fault_node.number << "\n";
          std::cout << "N faults spec0: "
                    << top_module_instance.spec_sbm0.hif_fault_node.number << "\n";
          std::cout << "N faults spec1: "
                    << top_module_instance.spec_sbm1.hif_fault_node.number << "\n";
          std::cout << "N faults spec2: "
                    << top_module_instance.spec_sbm2.hif_fault_node.number << "\n";

          std::vector<size_t> instanceToNumberOfFaults;
          instanceToNumberOfFaults.push_back(0);
instanceToNumberOfFaults.push_back(top_module_instance.hif_fault_node.number);
instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm0.hif_fault_node.number);
instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm1.hif_fault_node.number);
instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm2.hif_fault_node.number);
instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm3.hif_fault_node.number);
instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm4.hif_fault_node.number);
instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm5.hif_fault_node.number);

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
              top_module::top_module_iostruct in_rst_on;
              in_rst_on.clock = clock_0;
              top_module_instance.simulate(&in_rst_on, cycles_number);
              top_module_instance.initialize();
          
              top_module::top_module_iostruct in;
              bool faultFound = 0;
              for (size_t k = 0; k < traceLength; ++k) {
                clock_0 = !clock_0;
          
                in.clock = clock_0;
                // in
                if (!clock_0) {
                  setInputsFromTraceSample(in, golden_trace[k]);
                }
                top_module_instance.simulate(&in, cycles_number);
          
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