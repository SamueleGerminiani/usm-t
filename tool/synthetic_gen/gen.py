import subprocess
import os
import random
import copy 
import sys

root = os.environ["USMT_ROOT"]
yosis_prefix = root + '/tool/third_party/oss-cad-suite/bin/'
ltlsynt_prefix = root + '/tool/third_party/spot/bin/'
hif_tb_prefix = root + '/tool/synthetic_gen/hif_sim/'
xml_prefix = root +'/tool/synthetic_gen/specs/'
out_folder = root + '/tool/synthetic_gen/temp/'

def expand_spec(specification, lenght, assnumb):
    formula = specification['formula'] 
    #base name for the proposition is a_propertyIndex
    #ant_base = specification['inputs'] + "_" + str(assnumb)
    #con_base = specification['outputs'] + "_" + str(assnumb)
    #first propositions in the sequences are a_propertyIndex_0 and c_propertyIndex_0
    # + { is needed beacause we are using SERE syntax 
    ant_seq = "{" +  chr(97) + "_" + str(assnumb)
    con_seq = "{" + chr(97 + (lenght[0])) + "_" + str(assnumb)
    #initialize the inputs and outputs with the base propositions
    ins = "a_" + str(assnumb)
    outs = chr(97 + lenght[0]) + "_" + str(assnumb)

    if '..&&..' in formula:
        #expand the antecedent
        for i in range(1, lenght[0]):
            ant_base = chr(97 + i)  # 97 is the ASCII value for 'a'
            ins = ins + ',' + ant_base + "_" + str(assnumb)
            ant_seq = ant_seq + " & " + ant_base + "_" + str(assnumb)
        #expand the consequent
        for i in range(lenght[0] + 1, lenght[1]+lenght[0]):
            con_base = chr(97 + i)  # 97 is the ASCII value for 'a'
            outs = outs + ',' + con_base + "_" + str(assnumb)
            con_seq = con_seq + " & " + con_base + "_" + str(assnumb)
        #close the sequence with } for SERE syntax
        ant_seq = ant_seq + "}"
        con_seq = con_seq + "}" 
        # Replace only the first instance of '..&&..'
        formula = formula.replace('..&&..', ant_seq,1)
        # Replace the second instance of '..&&..'
        formula = formula.replace('..&&..', con_seq)
        
    if '..##1..' in formula:
        #expand the antecedent
        for i in range(1, lenght[0]):
            ant_base = chr(97 + i)  # 97 is the ASCII value for 'a'
            ins = ins + ',' + ant_base + "_" + str(assnumb)
            ant_seq = ant_seq + " ##1 " + ant_base + "_" + str(assnumb)
        #expand the consequent
        for i in range(lenght[0] + 1, lenght[1]+lenght[0]):
            con_base = chr(97 + i)  # 97 is the ASCII value for 'a'
            outs = outs + ',' + con_base + "_" + str(assnumb)
            con_seq = con_seq + " ##1 " + con_base + "_" + str(assnumb)

        #close the sequence with } for SERE syntax
        ant_seq = ant_seq + "}"
        con_seq = con_seq + "}" 
        # Replace '..##1..' in the antecedent
        formula = formula.replace('..##1..', ant_seq,1)
        # Replace '..##1..' in the consequent
        formula = formula.replace('..##1..',con_seq)
    #return the expanded formula
    ret_spec = {}
    ret_spec['formula'] = formula
    ret_spec['inputs'] = ins
    ret_spec['outputs'] = outs
    print(f"Expanded formula: {ret_spec['formula']}")
    return ret_spec

#Converts an AIGER file to a SystemVerilog file using Yosys
#design_aiger: the name of the AIGER file to convert
#specification: the specification dictionary containing the inputs and outputs. Needed to fix yosys nonsense
def aigerToSv(design_aiger, specification):
    input_file = design_aiger
    output_file = design_aiger.replace('.aiger', '.v')
    module_name = design_aiger.replace('.aiger', '')
    clk_name = 'clock'
    yosys_command = f"yosys -p 'read_aiger  -module_name {module_name} -clk_name {clk_name} {out_folder}{input_file}; write_verilog {out_folder}{output_file}'"
    subprocess.run(yosys_command, shell=True, check=False)

    #yosys for some reason mixes inputs and outputs in the module signal list so we need to fix it
    inputs = specification.get('inputs')
    outputs = specification.get('outputs')
    with open(out_folder + output_file, 'r') as file:   
        lines = file.readlines()
        # Find the module declaration line
        for i, line in enumerate(lines):
            if line.startswith('module '):
                module_decl = line.strip()
                break

        # Split the module declaration line into parts and get the module name
        parts = module_decl.split('(')
        module_name = parts[0]
        #signals = parts[1].rstrip(');').split(',')

        # Separate the clock, inputs, and outputs to create an ordered module port list
        new_ports = ['clock'] + inputs.split(',') + outputs.split(',')

        # Create the new module declaration line
        new_module_decl = module_name + '(' + ', '.join(new_ports) + ');'

        # Replace the old module declaration line with the new one
        lines[i] = new_module_decl + '\n'

        # Write the modified lines back to the file
        with open(out_folder + output_file, 'w') as file:
            file.writelines(lines)

    print(f"Generated SystemVerilog file: {output_file} in {out_folder}")

#Call ltlsynt to generate a controller from a specification
#The generated controller is saved in "aiger_filename" 
#aiger_filename is default to 'top_module.aiger' as the function will be called with a single parameter in the case of merged specifications.
#In the case of the submodules options being enabled, the function will be called with a second parameter to specify the name of the aiger file for each spec.
def synthesize_controller(specification, aiger_filename='top_module.aiger'):
    formula = specification.get('formula')
    inputs = specification.get('inputs')
    outputs = specification.get('outputs')
    
    ltlsynt_command = f'ltlsynt --formula="{formula}" --ins="{inputs}" --outs="{outputs}" --aiger > {out_folder}{aiger_filename}'

    result = subprocess.run(ltlsynt_command, shell=True, check=False, capture_output=True, text=True)
    if result.returncode == 1:
        print("Error: The design is unrealizable.")
        exit(1)
    elif result.returncode == 2:
        print("Error: An error occurred during the realizability check.")
        exit(2)
    else:
        print("Controller synthesized successfully.")
        #Remove REALIZABLE/UNREALIZABLE output line from aiger file
        with open(out_folder + aiger_filename, 'r') as file:
            lines = file.readlines()
        with open(out_folder + aiger_filename, 'w') as file:
            file.writelines(lines[1:])
        return aiger_filename

#Generates a top module that instantiates all the submodules
def generate_top_module(spec_list):
    #prefix of the top module 
    top_module = 'module top_module(\n'
    top_module += 'clock,' 
    #all submodule inputs
    for spec in spec_list:
        top_module += spec['inputs'] + ','
    #all submodule outputs
    for spec in spec_list:
        if(spec_list.index(spec) != len(spec_list) - 1):
            top_module += spec['outputs'] + ','
        else:
            top_module += spec['outputs'] + ');\n'
    #start input declaration
    top_module +='input clock,'
    for spec in spec_list:
        if(spec_list.index(spec) != len(spec_list) - 1):
            top_module += spec['inputs'] + ','
        else:
            top_module += spec['inputs'] + ';\n'
    #start output declaration
    top_module +='output '
    for spec in spec_list:
        if(spec_list.index(spec) != len(spec_list) - 1):
            top_module += spec['outputs'] + ','
        else:
            top_module += spec['outputs'] + ';\n'

    # instantiate the submodules
    for spec in spec_list:
        top_module += 'spec' + str(spec_list.index(spec)) + ' spec_sbm' + str(spec_list.index(spec)) + '(' + ".clock(clock), "
        for input_signal in spec['inputs'].split(','):
            top_module += f".{input_signal}({input_signal}), "
        for output_signal in spec['outputs'].split(','):
            top_module += f".{output_signal}({output_signal}), "
        top_module = top_module.rstrip(', ') + ');\n'

    top_module += 'endmodule\n'

    with open(out_folder + 'top_module.v', 'w') as file:
        file.write(top_module)
    print(f"Generated top_module module: {out_folder}top_module.v")

#Generates the circuit for the merged specification or for each submodule if the modules option is enabled
def generate_circuit(specification,spec_list, modules):
    if(modules):
        #We need to generate a module for each property
        for spec in spec_list:
            specfile_name = 'spec' + str(spec_list.index(spec))
            #for each specification generate a controller
            design_aiger = synthesize_controller(spec, specfile_name + '.aiger')
            #Generate a SystemVerilog file for each controller
            aigerToSv(design_aiger,spec)
        #Generate a top module that instantiates all the submodules
        generate_top_module(spec_list)
    else:
        #generate a single controller for the merged specification
        design_aiger = synthesize_controller(specification)
        #Generate a SystemVerilog file for the controller
        aigerToSv(design_aiger,specification)

#Generate testbench for Verilator
#def generate_testbench(specification):
    # inputs = specification.get('inputs', '').split(',')
    # testbench_code = ""
    # #create instructions to insert in tb
    # for signal in inputs:
    #     testbench_code += f"dut->{signal} = rand() % 2;\n\t\t"
    
    # print(f"Generated testbench file:\n {testbench_code}")
    
    # #retrieve the template for the testbench
    # with open(hif_tb_prefix + 'tb_test_template.txt', 'r') as template_file:
    #     template_content = template_file.read()

    # #substite the placeholder with the generated code
    # testbench_content = template_content.replace("PLACEHOLDER", testbench_code)

    # #write the testbench to a .cpp file
    # with open(hif_tb_prefix + 'tb_test.cpp', 'w') as testbench_file:
    #     testbench_file.write(testbench_content)

    #print(f"Generated testbench file: {hif_tb_prefix}tb_test.cpp")

#Generate testbench for HIFSuite
def generate_testbench(specification):
    #Get input and outputs
    inputs = specification.get('inputs', '').split(',')
    outputs = specification.get('outputs', '').split(',')
    
    #Testbench file includes and declarations
    declarations_string = "#include <stdlib.h>\n#include <algorithm>\n #include <cstdio>\n#include <filesystem>\n#include <fstream>\n#include <iostream>\n#include <vector>\n\n#include \"cpptracer/tracer.hpp\"\n#include \"muffin_dataTypes.hpp\"\n#include \"top_module.hpp\"\n\nusing Trace = std::vector<top_module::top_module_iostruct>;\n\n"
    
    #################################################
    #setRandomInputs function creation
    set_rand_input_string = "void setRandomInputs(top_module::top_module_iostruct& in) {\n"
    for signal in inputs:
        set_rand_input_string += f"\tin.{signal} = rand() % 2;\n"
    set_rand_input_string += "}"
    #################################################
    
    #################################################
    #setInputsFromTraceSample function creation
    set_inputs_from_trace_string = "void setInputsFromTraceSample(top_module::top_module_iostruct& in, const top_module::top_module_iostruct& dump) {\n"
    for signal in inputs:
        set_inputs_from_trace_string += f"\tin.{signal} = dump.{signal};\n"
    set_inputs_from_trace_string += "}"
    #################################################

    #################################################
    #checkOutputs function creation
    check_outputs_string = "bool checkOutput(const top_module::top_module_iostruct& golden, const top_module::top_module_iostruct& faulty) {\n"
    check_outputs_string += " if ("
    for signal in outputs:
        check_outputs_string += f"golden.{signal} != faulty.{signal} || "
    check_outputs_string = check_outputs_string[:-3] + ") {\n return 0;\n}\n return 1;\n}"
    #################################################

    #################################################
    #printSample function creation
    print_sample_string = "void printSample(top_module::top_module_iostruct& in) {"
    for signal in inputs:
        print_sample_string += f"printf(\"{signal}: %d\\n\", in.{signal});\n"
    print_sample_string += "}"   
    #################################################

    #################################################
    #we need to this to dump as a wire 1 bit long
    dump_string = "static std::vector<bool> vcd_clock = {0};\n "
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
    init_vcd_trace_string = "cpptracer::Tracer initVCDTrace(const std::string& name) {\n cpptracer::Tracer tracer(name, timeStep,\"top_module_bench\");\n tracer.addScope(\"top_module_\");\n\n"
    init_vcd_trace_string += "tracer.addTrace(vcd_clock, \"clock\");\n"
    for signal in inputs:
        init_vcd_trace_string += f"tracer.addTrace(vcd_{signal}, \"{signal}\");\n"
    for signal in outputs:
        init_vcd_trace_string += f"tracer.addTrace(vcd_{signal}, \"{signal}\");\n"

    init_vcd_trace_string += "tracer.closeScope();\n tracer.createTrace();\n return tracer;\n}"
    #################################################

    #################################################
    #updateVCDTrace function creation
    update_vcd_trace_string = "void updateVCDVariables(const top_module::top_module_iostruct& in) {\n vcd_clock[0] = in.clock;\n"
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
          size_t traceLength = 1000 * 2;  // 1000 positive and 1000 negative edges

          bool clock_0 = 0;

          printf("Simulate golden \\n");

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
                    << "\\n";

          std::cout << "N instances: " << muffin::hif_global_instance_counter << "\\n";
          std::cout << "N faults top_module: " << top_module_instance.hif_fault_node.number << "\\n";
          std::cout << "N faults spec0: "
                    << top_module_instance.spec_sbm0.hif_fault_node.number << "\\n";
          std::cout << "N faults spec1: "
                    << top_module_instance.spec_sbm1.hif_fault_node.number << "\\n";
          std::cout << "N faults spec2: "
                    << top_module_instance.spec_sbm2.hif_fault_node.number << "\\n";

          std::vector<size_t> instanceToNumberOfFaults;
          """
    
    #golden module 
    main_string += f"instanceToNumberOfFaults.push_back(0);\n"
    
    #top_module module
    main_string += f"instanceToNumberOfFaults.push_back(top_module_instance.hif_fault_node.number);\n"
    
    #if we have multiple submodules we need to inject faults in each of them
    spec_cpp_files = len([f for f in os.listdir(f"{hif_tb_prefix}injected/src") if f.endswith(".cpp") and f not in ["main.cpp", "hif_globals.cpp"]])
    for i in range(spec_cpp_files-1):
        main_string += f"instanceToNumberOfFaults.push_back(top_module_instance.spec_sbm{i}.hif_fault_node.number);\n"

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
    with open(hif_tb_prefix + 'injected/src/main.cpp', 'w') as testbench_file:
        testbench_file.write(testbench)


#Run verilator tb
def run_hifsuite(specification):
    #create rtl folder if needed
    rtl_folder = os.path.join(hif_tb_prefix, 'rtl')
    if not os.path.exists(rtl_folder):
        try:
            os.makedirs(rtl_folder)
            print(f"Created directory: {rtl_folder}")
        except OSError as e:
            print(f"Error: Failed to create directory {rtl_folder}. {e}")
            exit(1)

    #move all .v in outs folder to hif folder 
    result = subprocess.run(f"cp {out_folder}*.v {hif_tb_prefix}/rtl", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to copy .v files to hif_tb_prefix.")
        exit(1)
        
    # Collect all files that are not top_module.v
    #file_names = ""
    #for file in os.listdir(out_folder):
    #    if file.endswith(".v") and file != "top_module.v":
    #        file_names += file + " "

    #run hif pipeline
    generate_injectable_design()  

    #create testbench and setup the simulation directory
    generate_testbench(specification)
     
    #move the makfile to the simulation folder
    result = subprocess.run(f"cp {hif_tb_prefix}components/CMakeLists.txt {hif_tb_prefix}injected/", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to copy Makefile to hif_tb_prefix.")
        exit(1)

    #move cpptracer to the simulation folder
    result = subprocess.run(f"cp -r {hif_tb_prefix}components/cpptracer {hif_tb_prefix}injected/src", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to copy Makefile to hif_tb_prefix.")
        exit(1)

    #modify the CMakeLists.txt to include the eventual submodules
    submodule_string = "./src/top_module.cpp\n"
    spec_cpp_files = [f for f in os.listdir(f"{hif_tb_prefix}injected/src") if f.startswith("spec")]
    for spec_cpp_file in spec_cpp_files:
        submodule_string += f"./src/{spec_cpp_file}\n"
    with open (f"{hif_tb_prefix}injected/CMakeLists.txt", "r") as file:
        Cmake_template = file.read()    
    Cmake_template = Cmake_template.replace("./src/top_module.cpp", submodule_string)
    with open(f"{hif_tb_prefix}injected/CMakeLists.txt", "w") as file:
        file.write(Cmake_template)

    #Create the build directory and run cmake
    os.chdir(f"{hif_tb_prefix}injected")
    result = subprocess.run(f"mkdir build", shell=True, check=False, capture_output=True)
    if result.returncode != 0:
        if result.stderr.decode().find("File exists") != -1:
            print("Build directory already exists. Cleaning up...")
            result = subprocess.run(f"rm -rf build/*", shell=True, check=False)
            if result.returncode != 0:
                print("Error: Failed to clean build directory.")
                exit(1)
        else:
            print("Error: Failed to create build directory.")
            exit(1)    
    
    result = subprocess.run(f"cd build && cmake ..", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to run cmake.")
        exit(1)

    #compile the testbench
    result = subprocess.run(f"cd build && make", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed compile.")
        exit(1)

    #run the simulation
    result = subprocess.run("./build/synthetic_circuit", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to run the simulation.")
        exit(1)
    os.chdir(f"{root}/tool/synthetic_gen")

    #copy the trace file to the output_folder
    result = subprocess.run(f"cp {hif_tb_prefix}/injected/traces/*.vcd {out_folder}", shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to copy trace file to out_folder.")
        exit(1)

    #clean up the hif_sim folder
    subprocess.run(f"rm -rf {hif_tb_prefix}injected {hif_tb_prefix}traces {hif_tb_prefix}rtl {hif_tb_prefix}*.xml" , shell=True, check=False)

#Support function that calls the hif pipeline anche check for errors
def generate_injectable_design():
    # Set the LD_LIBRARY_PATH here because it conflicts with other commands
    verilog_files = subprocess.run(f"find {hif_tb_prefix}rtl/ -name '*.v'", shell=True, check=False, capture_output=True, text=True).stdout.strip().replace('\n', ' ')
    debug = True  # Set this to True to enable debug prints


    verilog2hif_command = f"verilog2hif {verilog_files} --output {hif_tb_prefix}synthetic_design.hif.xml"
    if debug:
        print(f"Running command: {verilog2hif_command}")
    result = subprocess.run(verilog2hif_command, stdout=subprocess.DEVNULL, shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to convert Verilog to HIF.")
        exit(1)

    ddt_command = f"ddt {hif_tb_prefix}synthetic_design.hif.xml --toplevel top_module --output {hif_tb_prefix}synthetic_design.ddt.hif.xml"
    if debug:
        print(f"Running command: {ddt_command}")
    result = subprocess.run(ddt_command, stdout=subprocess.DEVNULL, shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to run ddt on synthetic_design.hif.xml.")
        exit(1)
    
    a2tool_command = f"a2tool {hif_tb_prefix}synthetic_design.ddt.hif.xml --protocol CPP --toplevel top_module --output {hif_tb_prefix}synthetic_design.ddt.a2t.hif.xml"
    if debug:
        print(f"Running command: {a2tool_command}")
    result = subprocess.run(a2tool_command, stdout=subprocess.DEVNULL, shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to run a2tool on synthetic_design.ddt.hif.xml.")
        exit(1)
    
    muffin_command = f"muffin {hif_tb_prefix}synthetic_design.ddt.a2t.hif.xml --fault STUCK_AT --clock clock --toplevel top_module --output {hif_tb_prefix}synthetic_design.ddt.a2t.muffin.hif.xml"
    if debug:
        print(f"Running command: {muffin_command}")
    result = subprocess.run(muffin_command, stdout=subprocess.DEVNULL, shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to run muffin on synthetic_design.ddt.a2t.hif.xml.")
        exit(1)
    
    hif2sc_command = f"hif2sc {hif_tb_prefix}synthetic_design.ddt.a2t.muffin.hif.xml --extensions cpp_hpp --directory {hif_tb_prefix}injected"
    if debug:
        print(f"Running command: {hif2sc_command}")
    result = subprocess.run(hif2sc_command, stdout=subprocess.DEVNULL, shell=True, check=False)
    if result.returncode != 0:
        print("Error: Failed to run hif2sc on synthetic_design.ddt.a2t.muffin.hif.xml.")
        exit(1)

    #unset the LD_LIBRARY_PATH to avoid conflicts

    print("HIF_SIM: sussessfully generated injectable design \n")

#Support function that moves the generated files to the input folder
def populate_input_dir():
    input_prefix = root + '/input/synthetic'
    subprocess.run(f"mv {out_folder}*.v {input_prefix}/design/", shell=True, check=False)
    subprocess.run(f"mv {out_folder}specifications.txt {input_prefix}/expected/", shell=True, check=False)
    subprocess.run(f"mv {out_folder}*.vcd {input_prefix}/traces/", shell=True, check=False)
    subprocess.run(f"rm -rf {out_folder}/*", shell=True, check=False)

def main():
    import xml.etree.ElementTree as ET
    # xml_file = f'{xml_prefix}output_templates.xml'
    # tree = ET.parse(xml_file)
    # root = tree.getroot()
    # templates = root.findall('Template')
    # #This will be used when everything works and we will have to select the templates
    # num_templates = len(templates)

    #input parameters
    if len(sys.argv) != 2:
        print("Usage: python gen.py <config_file>")
        exit(1)
    config_file = sys.argv[1]
    config_tree = ET.parse(config_file)
    config_root = config_tree.getroot()

    template_number = int(config_root.find('parameter').attrib['ntemplates'])
    ant_props = int(config_root.find('parameter').attrib['nant'])
    con_props = int(config_root.find('parameter').attrib['ncon'])
    numprops = (ant_props, con_props)
    assnumbs = int(config_root.find('parameter').attrib['nspec'])
    modules = config_root.find('parameter').attrib['parallel'] == "1"
    templates = config_root.findall('template')
    


    merged_specification = {}
    subprocess.run(f'touch {out_folder}/specifications.txt', shell=True, check=False)

    #This is used to count the global number of specifications thata has been generated so far
    specounter = 0

    #randomly select template_number templates
    random_templates = random.sample(templates, template_number)
    #iterate over the selected templates to expand them and merge them
    for i, template in enumerate(random_templates, start=1):
        specification = {}
        specification['formula'] = template.attrib['text']
        specification['inputs'] = template.attrib['ins']
        specification['outputs'] = template.attrib['outs']
        
        spec_list = []
        #TODO: this works only for multiple instances of the same template, if we get multiple templates we need to share assnumbs between them or just ignore it and have numtemplates*numassertions specifications
        for j, num in enumerate(range(1, assnumbs + 1), start=1):
            #expand special templates
            if(specification['formula'].find('..##1..') or specification['formula'].find('..&&..')):
                print(f"Expanding template")  
                expanded_formula = expand_spec(specification,numprops,specounter)
            else: 
                expanded_formula = specification
            # update merged_specification structure
            if(merged_specification.get('formula') == None):
                merged_specification = expanded_formula
            else:  
                merged_specification['formula'] = expanded_formula['formula'] + ' & ' + merged_specification.get('formula', '')
                merged_specification['inputs'] = ','.join(set(merged_specification.get('inputs', '').split(',') + expanded_formula['inputs'].split(',')))
                merged_specification['outputs'] = ','.join(set(merged_specification.get('outputs', '').split(',') + expanded_formula['outputs'].split(',')))
    
            # Write expanded formulas to a file
            with open(out_folder + 'specifications.txt', 'a') as file:
            #    file.write(f"Expanded formula {j} for template {i}:\n")
                file.write(f"{expanded_formula['formula']}\n\n")

            #if module subdivision is enabled
            if(modules):
                #for each iteration add the expanded formula to the list
                spec_list.append(copy.deepcopy(expanded_formula))
                
            #update the global spec counter    
            specounter += 1

    # Write merged specification to a file
    #with open(out_folder + 'specifications.txt', 'a') as file:
        #file.write("Merged specification:\n")
        #file.write(f"Formula: {merged_specification['formula']}\n")
        #file.write(f"Inputs: {merged_specification['inputs']}\n")
        #file.write(f"Outputs: {merged_specification['outputs']}\n")
    
    print("Merged specification:\n")
    print(merged_specification)
    print("Generating circuit for merged specification")
    generate_circuit(merged_specification,spec_list, modules)
    run_hifsuite(merged_specification)
    populate_input_dir()

if __name__ == "__main__":
    main()