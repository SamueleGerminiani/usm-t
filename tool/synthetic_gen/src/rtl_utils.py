import subprocess
import globals
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
        design_aiger = synthesize_controller(specification, f"{globals.top_module_name}.aiger")
        #Generate a SystemVerilog file for the controller
        aigerToSv(design_aiger,specification)
    if globals.debug: 
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("Circuit generated! \n")


#Generates a top module that instantiates all the submodules
def generate_top_module(spec_list):

    #prefix of the top module 
    top_module = f'module {globals.top_module_name}('
    top_module += f'{globals.clk_name},' 
    #all submodule inputs
    inputs_tmp = []
    outs_tmp = []
    for spec in spec_list:
        inputs_tmp.extend(spec['inputs'].split(','))    
    #all submodule outputs
    top_module += ",".join(set(inputs_tmp)) + ','
    for spec in spec_list:
        outs_tmp.extend(spec['outputs'].split(','))    
    top_module += ",".join(set(outs_tmp)) + ');\n'

    #start input declaration
    top_module +=f'input {globals.clk_name},'
    top_module += ",".join(set(inputs_tmp)) + ';\n'
    #start output declaration
    top_module +='output '
    top_module += ",".join(set(outs_tmp)) + ';\n'

    # instantiate the submodules
    for spec in spec_list:
        top_module += 'spec' + str(spec_list.index(spec)) + ' spec_sbm' + str(spec_list.index(spec)) + '(' + f".{globals.clk_name}({globals.clk_name}), "
        for input_signal in spec['inputs'].split(','):
            top_module += f".{input_signal}({input_signal}), "
        for output_signal in spec['outputs'].split(','):
            top_module += f".{output_signal}({output_signal}), "
        top_module = top_module.rstrip(', ') + ');\n'

    top_module += 'endmodule\n'

    with open(globals.out_folder + f'{globals.top_module_name}.v', 'w') as file:
        file.write(top_module)
    if globals.debug:    
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Generated {globals.top_module_name} module: {globals.out_folder}{globals.top_module_name}.v")


#Call ltlsynt to generate a controller from a specification
#The generated controller is saved in "aiger_filename" 
#aiger_filename is default to 'top_module.aiger' as the function will be called with a single parameter in the case of merged specifications.
#In the case of the submodules options being enabled, the function will be called with a second parameter to specify the name of the aiger file for each spec.
def synthesize_controller(specification, aiger_filename):
    formula = specification.get('formula')
    inputs = specification.get('inputs')
    outputs = specification.get('outputs')
    
    ltlsynt_command = f'ltlsynt --formula="{formula}" --ins="{inputs}" --outs="{outputs}" --aiger > {globals.out_folder}{aiger_filename}'

    result = subprocess.run(ltlsynt_command, shell=True, check=False, capture_output=True, text=True)
    if result.returncode == 1:
        print(globals.CERR +"Error:" + globals.CEND + "The design is unrealizable.")
        exit(1)
    elif result.returncode == 2:
        print(globals.CERR +"Error:" + globals.CEND + "An error occurred during the realizability check.")
        exit(2)
    else:
        if globals.debug:
            print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
            print("Controller synthesized successfully. \n")
        #Remove REALIZABLE/UNREALIZABLE output line from aiger file
        with open(globals.out_folder + aiger_filename, 'r') as file:
            lines = file.readlines()
        with open(globals.out_folder + aiger_filename, 'w') as file:
            file.writelines(lines[1:])
        return aiger_filename


#Converts an AIGER file to a SystemVerilog file using Yosys
#design_aiger: the name of the AIGER file to convert
#specification: the specification dictionary containing the inputs and outputs. Needed to fix yosys nonsense
def aigerToSv(design_aiger, specification):
    input_file = design_aiger
    output_file = design_aiger.replace('.aiger', '.v')
    module_name = design_aiger.replace('.aiger', '')
    yosys_command = f"yosys -p 'read_aiger  -module_name {module_name} -clk_name {globals.clk_name} {globals.out_folder}{input_file}; write_verilog {globals.out_folder}{output_file}'"
    try:    
        subprocess.run(yosys_command, stdout=subprocess.DEVNULL, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to convert AIGER to SystemVerilog. errno: {e.returncode}")
        exit(1)

    #yosys for some reason mixes inputs and outputs in the module signal list so we need to fix it
    inputs = specification.get('inputs')
    outputs = specification.get('outputs')
    with open(globals.out_folder + output_file, 'r') as file:   
        lines = file.readlines()
        # Find the module declaration line
        for i, line in enumerate(lines):
            if line.startswith('module '):
                module_decl = line.strip()
                break

        # Split the module declaration line into parts and get the module name
        parts = module_decl.split('(')
        module_name = parts[0]

        # Separate the clock, inputs, and outputs to create an ordered module port list
        new_ports = [globals.clk_name] + inputs.split(',') + outputs.split(',')

        # Create the new module declaration line
        new_module_decl = module_name + '(' + ', '.join(new_ports) + ');'

        # Replace the old module declaration line with the new one
        lines[i] = new_module_decl + '\n'

        # Add clock input and wire declarations if not already present because yosys in non deterministic
        if f'input {globals.clk_name};' not in lines:
            lines.insert(i + 1, f'input {globals.clk_name};\n')
        if f'wire {globals.clk_name};' not in lines:
            lines.insert(i + 2, f'wire {globals.clk_name};\n')

        # Write the modified lines back to the file
        with open(globals.out_folder + output_file, 'w') as file:
            file.writelines(lines)

    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Generated SystemVerilog file: {output_file} in {globals.out_folder} \n")
