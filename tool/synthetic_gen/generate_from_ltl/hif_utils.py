import os
import subprocess
import globals

#Support function that calls the hif pipeline anche check for errors
def generate_injectable_design():
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("Generating injectable design \n")

    # Set the LD_LIBRARY_PATH here because it conflicts with other commands
    verilog_files = subprocess.run(f"find {globals.hif_temp_prefix}rtl/ -name '*.v'", shell=True, check=False, capture_output=True, text=True).stdout.strip().replace('\n', ' ')

    verilog2hif_command = f"verilog2hif {verilog_files} --output {globals.hif_temp_prefix}synthetic_design.hif.xml"
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Running command: {verilog2hif_command} \n")
    try:    
        subprocess.run(verilog2hif_command,stderr=subprocess.DEVNULL if not globals.debug else None,  stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run verilog2hif. {e}")
        exit(1)

    ddt_command = f"ddt {globals.hif_temp_prefix}synthetic_design.hif.xml --toplevel {globals.top_module_name} --output {globals.hif_temp_prefix}synthetic_design.ddt.hif.xml"
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Running command: {ddt_command} \n")
    try: 
        subprocess.run(ddt_command,stderr=subprocess.DEVNULL if not globals.debug else None,  stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run ddt. {e}")
        exit(1)
    a2tool_command = f"a2tool {globals.hif_temp_prefix}synthetic_design.ddt.hif.xml --protocol CPP --toplevel {globals.top_module_name} --output {globals.hif_temp_prefix}synthetic_design.ddt.a2t.hif.xml"
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Running command: {a2tool_command} \n")
    try:
        subprocess.run(a2tool_command,stderr=subprocess.DEVNULL if not globals.debug else None, stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run a2tool. {e}")
        exit(1)
    
    muffin_command = f"muffin {globals.hif_temp_prefix}synthetic_design.ddt.a2t.hif.xml --fault STUCK_AT --clock {globals.clk_name} --toplevel {globals.top_module_name} --output {globals.hif_temp_prefix}synthetic_design.ddt.a2t.muffin.hif.xml"
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Running command: {muffin_command} \n")
    try:
        subprocess.run(muffin_command,stderr=subprocess.DEVNULL if not globals.debug else None, stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run muffin. {e}")
        exit(1)
    
    hif2sc_command = f"hif2sc {globals.hif_temp_prefix}synthetic_design.ddt.a2t.muffin.hif.xml --extensions cpp_hpp --directory {globals.hif_temp_prefix}injected"
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Running command: {hif2sc_command} \n")
    try:
        subprocess.run(hif2sc_command,stderr=subprocess.DEVNULL if not globals.debug else None, stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run hif2sc. {e}")
        exit(1)

    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("HIF: sussessfully generated injectable design \n")


#Run hif tb
def run_hifsuite(specification):
    os.makedirs(globals.hif_temp_prefix, exist_ok=True)


    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("Performing Fault injection and simulation \n")

    #create rtl folder if needed
    rtl_folder = os.path.join(globals.hif_temp_prefix, 'rtl')
    #delete the folder if it already exists
    if os.path.exists(rtl_folder):
        if globals.debug:
            print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
            print(f"Removing existing rtl directory\n")
        try:
            subprocess.run(f"rm -rf {rtl_folder}", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to remove rtl directory. {e}")
            exit(1)
    try:
        os.makedirs(rtl_folder)
        if globals.debug:
            print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
            print(f"Created directory: {rtl_folder}")
    except OSError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {rtl_folder}. {e}")
        exit(1)

    #move all .v in outs folder to hif folder 
    try:
        subprocess.run(f"cp {globals.out_folder}*.v {globals.hif_temp_prefix}/rtl", stdout=subprocess.DEVNULL, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to copy files to {globals.hif_temp_prefix}/rtl. {e}")
        exit(1)    
   
    #run hif pipeline
    generate_injectable_design()  

    #create testbench and setup the simulation directory
    generate_testbench(specification)
     
    #copy the makfile to the simulation folder
    try:
        subprocess.run(f"cp {globals.hif_template_prefix}/CMakeLists.txt {globals.hif_temp_prefix}injected/", stdout=subprocess.DEVNULL, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to copy Makefile to {globals.hif_temp_prefix}. {e}")
        exit(1)
    #copy cpptracer to the simulation folder
    try:
        subprocess.run(f"cp -r {globals.hif_template_prefix}/cpptracer {globals.hif_temp_prefix}injected/src", stdout=subprocess.DEVNULL, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to copy cpptracer to {globals.hif_temp_prefix}. {e}")
        exit(1)

    #modify the CMakeLists.txt to include the eventual submodules
    submodule_string = f"./src/{globals.top_module_name}.cpp"+"\n"
    spec_cpp_files = [f for f in os.listdir(f"{globals.hif_temp_prefix}injected/src") if f.startswith("spec")]
    for spec_cpp_file in spec_cpp_files:
        submodule_string += f"./src/{spec_cpp_file}"+"\n"
    with open (f"{globals.hif_temp_prefix}injected/CMakeLists.txt", "r") as file:
        Cmake_template = file.read()    
    Cmake_template = Cmake_template.replace(f"./src/top_module.cpp", submodule_string)
    with open(f"{globals.hif_temp_prefix}injected/CMakeLists.txt", "w") as file:
        file.write(Cmake_template)

    #Create the build directory and run cmake
    os.chdir(f"{globals.hif_temp_prefix}injected")

    if os.path.exists("build"):
        if globals.debug:
            print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
            print("Removing existing build directory\n")
        try:
            subprocess.run(f"rm -rf build", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to remove build directory. {e}")
            exit(1)
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("Creating build directory\n")        
    try:
        subprocess.run(f"mkdir build", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to create build directory. {e}")
        exit(1)

    
    try:
        subprocess.run(f"cd build && cmake ..",stderr=subprocess.DEVNULL if not globals.debug else None, stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run cmake. {e}")
        exit(1)

    #compile the testbench
    try:
        subprocess.run(f"cd build && make",stderr=subprocess.DEVNULL if not globals.debug else None, stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to compile the testbench. {e}")
        exit(1)

    #run the simulation
    try:
        subprocess.run(f"./build/synthetic_circuit",stderr=subprocess.DEVNULL if not globals.debug else None, stdout=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to run the simulation. {e}")
        exit(1)

    os.chdir(f"{globals.root}/tool/synthetic_gen")

    #copy the trace file to the output_folder
    try:
        subprocess.run(f"cp {globals.hif_temp_prefix}/injected/traces/*.vcd {globals.out_folder}faulty_traces/vcd", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to copy trace file to {globals.out_folder}. {e}")
        exit(1)


    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("Design injected and simulated correctly \n")


#Generate testbench for HIFSuite
def generate_testbench(specification):
    #Get input and outputs
    inputs = specification.get('inputs', '').split(',')
    outputs = specification.get('outputs', '').split(',')
    
    #Testbench file includes and declarations
    declarations_string = f"#include <stdlib.h>\n#include <algorithm>\n #include <cstdio>\n#include <filesystem>\n#include <fstream>\n#include <iostream>\n#include <vector>\n\n#include \"cpptracer/tracer.hpp\"\n#include \"muffin_dataTypes.hpp\"\n#include \"{globals.top_module_name}.hpp\"\n\nusing Trace = std::vector<{globals.top_module_name}::{globals.top_module_name}_iostruct>;\n\n"
    
    #################################################
    #setRandomInputs function creation
    set_rand_input_string = f"void setRandomInputs({globals.top_module_name}::{globals.top_module_name}_iostruct& in)"+"{\n"
    for signal in inputs:
        set_rand_input_string += f"\tin.{signal} = rand() % 2;\n"
    set_rand_input_string += "}"
    #################################################
    
    #################################################
    #setInputsFromTraceSample function creation
    set_inputs_from_trace_string = f"void setInputsFromTraceSample({globals.top_module_name}::{globals.top_module_name}_iostruct& in, const {globals.top_module_name}::{globals.top_module_name}_iostruct& dump)"+" {\n"
    for signal in inputs:
        set_inputs_from_trace_string += f"\tin.{signal} = dump.{signal};\n"
    set_inputs_from_trace_string += "}"
    #################################################

    #################################################
    #checkOutputs function creation
    check_outputs_string = f"bool checkOutput(const {globals.top_module_name}::{globals.top_module_name}_iostruct& golden, const {globals.top_module_name}::{globals.top_module_name}_iostruct& faulty)"+" {\n"
    check_outputs_string += " if ("
    for signal in outputs:
        check_outputs_string += f"golden.{signal} != faulty.{signal} || "
    check_outputs_string = check_outputs_string[:-3] + ") {\n return 0;\n}\n return 1;\n}"
    #################################################

    #################################################
    #printSample function creation
    print_sample_string = f"void printSample({globals.top_module_name}::{globals.top_module_name}_iostruct& in)"+" {"
    for signal in inputs:
        print_sample_string += f"printf(\"{signal}: %d\\n\", in.{signal});\n"
    print_sample_string += "}"   
    #################################################

    #################################################
    #we need to this to dump as a wire 1 bit long
    dump_string = f"static std::vector<bool> vcd_{globals.clk_name}"+"= {0};\n "
    zero = '{0}'
    for signal in inputs:
        dump_string += f"static std::vector<bool> vcd_{signal} = {zero};\n"
    for signal in outputs:  
        dump_string += f"static std::vector<bool> vcd_{signal} = {zero};\n"
    #################################################

    #################################################
    #timestep declaration
    timestep_string = "static cpptracer::TimeScale timeStep(1, cpptracer::TimeUnit::NS);\n"
    #################################################

    #################################################
    #initVCDTrace function creation
    init_vcd_trace_string = "cpptracer::Tracer initVCDTrace(const std::string& name) {\n cpptracer::Tracer tracer(name, timeStep,"+f"\"{globals.top_module_name}_bench\");\n tracer.addScope(\"{globals.top_module_name}_\");\n\n"
    init_vcd_trace_string += f"tracer.addTrace(vcd_{globals.clk_name}, \"{globals.clk_name}\");\n"
    for signal in inputs:
        init_vcd_trace_string += f"tracer.addTrace(vcd_{signal}, \"{signal}\");\n"
    for signal in outputs:
        init_vcd_trace_string += f"tracer.addTrace(vcd_{signal}, \"{signal}\");\n"

    init_vcd_trace_string += "tracer.closeScope();\n tracer.createTrace();\n return tracer;\n}"
    #################################################

    #################################################
    #updateVCDTrace function creation
    update_vcd_trace_string = f"void updateVCDVariables(const {globals.top_module_name}::{globals.top_module_name}_iostruct& in)"+" {\n"+ f"vcd_{globals.clk_name}[0] = in." + f"{globals.clk_name};\n"
    for signal in inputs:
        update_vcd_trace_string += f"vcd_{signal}[0] = in.{signal};\n"
    for signal in outputs:
        update_vcd_trace_string += f"vcd_{signal}[0] = in.{signal};\n"
    update_vcd_trace_string += "}"
    #################################################

    #################################################
    #areEquivalent function creation
    are_equivalent_string = "bool areEquivalent(const Trace& t1, const Trace& t2) {\n"
    are_equivalent_string += "if (t1.size() != t2.size()) {\n return false;\n}\n"
    are_equivalent_string += "for (size_t i = 0; i < t1.size(); ++i) {\n"
    are_equivalent_string += "if ("
    for signal in inputs:
        are_equivalent_string += f"t1[i].{signal} != t2[i].{signal} || "
    for signal in outputs:
        are_equivalent_string += f"t1[i].{signal} != t2[i].{signal} || "
    are_equivalent_string = are_equivalent_string[:-3] + ") {\n return false;\n}\n}\n return true;\n}"
    #################################################

    #################################################
    #main function creation
    main_string = "int main() {\n"
    #main function body hardcoded in the script as it can remain the same for all the testbenches
    main_string += """
          muffin::stuck_at = 0;
          muffin::hif_global_instance_counter = 0;

          // number of faulted instances created with muffin, 0 is the original design
          // without faults
          muffin::instance_number = 0;

          // faults
          int32_t cycles_number = 0;
          size_t traceLength = """ + f"{globals.tracelnegth}" + """ * 2;  // 1000 positive and 1000 negative edges

          bool """ + f"{globals.clk_name}"+"""_0 = 0;

          printf("Simulate golden \\n");
          
          """ + f"{globals.top_module_name} {globals.top_module_name}_instance;\n {globals.top_module_name}_instance.initialize();" + """
          Trace golden_trace;

          // in case of a rst
          """ + f"{globals.top_module_name}::{globals.top_module_name}_iostruct in_rst_on;" + """
          """ + f"in_rst_on.{globals.clk_name} = {globals.clk_name}_0;\n" + """
          """ + f"{globals.top_module_name}_instance.simulate(&in_rst_on, cycles_number);" + """
          """ + f"{globals.top_module_name}_instance.initialize();" + """

          srand(0);
          """ + f"{globals.top_module_name}::{globals.top_module_name}_iostruct in;" + """

          for (size_t k = 0; k < traceLength; ++k) {
        """ +    f"{globals.clk_name}_0 = !{globals.clk_name}_0;" + """

        """ +  f"in.{globals.clk_name} = {globals.clk_name}_0;" + """

            // in
        """ +  f" if (!{globals.clk_name}_0)" + """ { 
              setRandomInputs(in);
            }
        
        """ + f"{globals.top_module_name}_instance.simulate(&in, cycles_number);" + """

            // out
            golden_trace.push_back(in);
            // printSample(in);
          }
          // Simulate the design with faults (instance 1 to 4), 1 is top_module with 0 faults
          // input trace
          size_t faultObserved = 0;
          std::vector<std::pair<size_t, size_t>> uncoveredFaults;
          std::vector<Trace> faultyTraces;
          std::cout << "Number of faults: " << """ + f"{globals.top_module_name}_instance.hif_fault_node.number" + """
                    << "\\n";

          std::vector<size_t> instanceToNumberOfFaults;
          """
    
    #golden module 
    main_string += f"instanceToNumberOfFaults.push_back(0);\n"
    
    #top_module module
    main_string += f"instanceToNumberOfFaults.push_back({globals.top_module_name}_instance.hif_fault_node.number);\n"
    
    #if we have multiple submodules we need to inject faults in each of them
    spec_cpp_files = len([f for f in os.listdir(f"{globals.hif_temp_prefix}injected/src") if f.endswith(".cpp") and f not in ["main.cpp", "hif_globals.cpp"]])
    for i in range(spec_cpp_files-1):
        main_string += f"instanceToNumberOfFaults.push_back({globals.top_module_name}_instance.spec_sbm{i}.hif_fault_node.number);\n"

    main_string +="""
          for (size_t curr_instance_number = 1;
               curr_instance_number <= instanceToNumberOfFaults.size() - 1;
               ++curr_instance_number) {
            muffin::instance_number = curr_instance_number;
            size_t nFaults = instanceToNumberOfFaults[curr_instance_number];
          
            for (muffin::fault_number = 0; muffin::fault_number < nFaults;
                 ++muffin::fault_number) {
              Trace faulty_trace;
              printf("Simulating fault number '%ld:%ld'\\n", muffin::instance_number,
                     muffin::fault_number);
          
            """ + f"{globals.clk_name}_0 = 0;" + """
            """ + f"{globals.top_module_name}::{globals.top_module_name}_iostruct in_rst_on;" + """
            """ + f"in_rst_on.{globals.clk_name} = {globals.clk_name}_0;" + """
            """ + f"{globals.top_module_name}_instance.simulate(&in_rst_on, cycles_number);" + """
            """ + f"{globals.top_module_name}_instance.initialize();" + """
          
            """ + f"{globals.top_module_name}::{globals.top_module_name}_iostruct in;" + """
              bool faultFound = 0;
              for (size_t k = 0; k < traceLength; ++k) {
                """ + f"{globals.clk_name}"+"""_0 = !""" + f"{globals.clk_name}"+"""_0;
          
            """ + f"in.{globals.clk_name} = {globals.clk_name}_0;" + """
                // in
                if (!""" + f"{globals.clk_name}"+"""_0) {
                  setInputsFromTraceSample(in, golden_trace[k]);
                }
            """ + f"{globals.top_module_name}_instance.simulate(&in, cycles_number);" + """
          
                faulty_trace.push_back(in);
          
                // get the output
                if (""" + f"{globals.clk_name}"+"""_0) {
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

          printf("Faults observed: %ld\\n", faultObserved);
          for (auto fault : uncoveredFaults) {
            printf("Fault n %ld not observed\\n", fault);
          }
          printf("Unique faulty traces: %ld\\n", faultyTraces.size());

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
        }"""

    #concatenate all the strings to create the testbench
    testbench = declarations_string + "\n" +  set_rand_input_string + "\n" + set_inputs_from_trace_string + "\n" + check_outputs_string + "\n" + print_sample_string  + "\n"+ dump_string  + "\n"+ timestep_string + "\n"+ init_vcd_trace_string + "\n"+ update_vcd_trace_string + "\n"+ are_equivalent_string + "\n"+ main_string
    #write the testbench to a .cpp file
    with open(globals.hif_temp_prefix + 'injected/src/main.cpp', 'w') as testbench_file:
        testbench_file.write(testbench)

    if globals.debug: 
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Generated testbench: {globals.hif_temp_prefix}injected/src/main.cpp \n")
