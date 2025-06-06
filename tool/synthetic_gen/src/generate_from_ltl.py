import re
import subprocess
import os
import random
import copy 
import sys
import argparse
import dir_utils
import hif_utils
import rtl_utils
import globals

#This needs to be changed to adapt to all the possible templates or use a real parser to parse the LTL formula
#This function expands the special templates in the formula
#The special templates are:
#G(ant -> con)
#G(..##N.. -> ..##M..)
#G(..&&.. -> ..&&..)
#G(..#N&.. -> ..#M&..)
#G(..&&.. -> ..&&.. U ..&&..)
#G(..&&.. -> F ..&&..)
#numprops is a pair of max numprops in the antecedent and max number of propositions in the consequent
def expand_spec(specification, numprops, assnumb):
    formula = specification['formula'] 
    # Find all occurrences of '..&&..' or '..##N..' in the formula
    #Need to use regex because the placeholders can have arbitrary numbers
    placeholder_positions = {}
    match = re.search(r'\(([^)]*)\)', formula)
    formula_reduced = match.group(1) if match else formula
    formula_parts = formula_reduced.split(' ')
    implication_index = 0
    #find placeholders in the formula and store their positions
    for i, part in enumerate(formula_parts):
        if re.fullmatch(r'..&&..', part) or re.fullmatch(r"..##\d+..", part) or re.fullmatch(r'..#\d+&..', part):
            placeholder_positions[i] = part
        if '>' in part:    
            implication_index = i
    if placeholder_positions == {}:
        print(globals.CERR +"Error:" + globals.CEND + "No placeholders found in the formula. Please use '..&&..', '..##N..' or '..#N&..' as placeholders.")
        exit(1)

    # Expand the placeholders in the formula
    # If we have two placeholders we will have only one antecedent and consequent sequence
    # If we have three placeholders we will have two consequent sequences
    ins = ""
    outs = "" 
    prop_seq = {}
    
    #97 because we start from 'a' in ASCII
    current_prop = 97
    for pos, ph in placeholder_positions.items():
        # Select the sequence length based on the position of the placeholder (ant or cons)
        if implication_index > pos:
            seq_len_selector = 0
        else:
            seq_len_selector = 1
        match ph:
            case '..&&..':
                prop_seq[pos] = "{"
                for i in range(0, numprops[seq_len_selector]):
                    prop_seq[pos] += chr(current_prop + i) + "_" + str(assnumb) + " & "
                    if implication_index > pos:
                        ins += chr(current_prop + i) + "_" + str(assnumb) + ","
                    else:
                        outs += chr(current_prop + i) + "_" + str(assnumb) + ","
                prop_seq[pos] = prop_seq[pos][:-2] + "}"
                current_prop += i + 1
                
            case ph if re.fullmatch(r'..##\d+..', ph):
                # Extract the number between "##" and ".."
                number = ph[4:-2]
                prop_seq[pos] = "{"
                for i in range(0, numprops[seq_len_selector]):
                    prop_seq[pos] += chr(current_prop + i) + "_" + str(assnumb) + " ##" + number + " "
                    if implication_index > pos:
                        ins += chr(current_prop + i) + "_" + str(assnumb) + ","
                    else:
                        outs += chr(current_prop + i) + "_" + str(assnumb) + ","
                prop_seq[pos] = prop_seq[pos][:-4] + "}" 
                current_prop += i + 1

            case ph if re.fullmatch(r'..#\d+&..', ph):
                # Extract the number between "#" and "&"
                number = ph[3:-3]
                prop_seq[pos] = "{"
                operators = ["&", "##"+str(number)]
                for i in range(0, numprops[seq_len_selector]):
                    prop_seq[pos] += chr(current_prop + i) + "_" + str(assnumb) + " " + operators[i % 2] + " "
                    if implication_index > pos:
                        ins += chr(current_prop + i) + "_" + str(assnumb) + ","
                    else:
                        outs += chr(current_prop + i) + "_" + str(assnumb) + ","
                if i % 2 == 1:
                    prop_seq[pos] = prop_seq[pos][:-4] + "}"
                else:
                    prop_seq[pos] = prop_seq[pos][:-2] + "}"
                current_prop += i + 1
            case _:
                pass

    ins = ins[:-1]            
    outs = outs[:-1]
    #replace the placeholders in the formula
    for pos, ph in placeholder_positions.items():
        formula_parts[pos] = prop_seq[pos]
    formula = "G(" + ' '.join(formula_parts) + ")"
    #return the expanded formula
    ret_spec = {}
    ret_spec['formula'] = formula
    ret_spec['inputs'] = ins
    ret_spec['outputs'] = outs
    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print(f"Expanded formula: {ret_spec['formula']}")
    return ret_spec

#This function creates the specifications from the templates
def create_specification(template_list,modules):
    merged_specification = {}
    #This will be used in case of module subdivision
    spec_list = []
    #This is used to count the global number of specifications thata has been generated so far
    specounter = 0

    #iterate over the templates to expand them and merge them
    for i, template in enumerate(template_list, start=0):
        specification = {}
        #Get the template string
        specification['formula'] = template['formula']
        #Get the overlap value  
        specification['overlap'] = template['overlap']
        #Get the number of propositions in the antecedent and consequent
        numprops = (int(template['nant']), int(template['ncon']))

        #TODO: this works only for multiple instances of the same template, if we get multiple templates we need to share assnumbs between them or just ignore it and have numtemplates*numassertions specifications
        for j, num in enumerate(range(1, template['nspec'] + 1), start=1):
            #expand special templates
            if '..' in specification['formula']:
                if globals.debug:
                    print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
                    print(f"Expanding template")

                specification['inputs'] = ""
                specification['outputs'] = "" 
                expanded_formula = expand_spec(specification,numprops,specounter)
            else: 
                print(globals.CERR +"Error:" + globals.CEND + f"Template {i} not recognized. Please use '..##1..' or '..&&..' in the template string")   
                exit(1)

            # update merged_specification structure
            if(merged_specification.get('formula') == None):
                merged_specification = expanded_formula
            else:  
                merged_specification['formula'] = expanded_formula['formula'] + ' & ' + merged_specification.get('formula', '')
                merged_specification['inputs'] = ','.join(set(merged_specification.get('inputs', '').split(',') + expanded_formula['inputs'].split(',')))
                merged_specification['outputs'] = ','.join(set(merged_specification.get('outputs', '').split(',') + expanded_formula['outputs'].split(',')))

            #for each iteration add the expanded formula to the list
            spec_list.append(copy.deepcopy(expanded_formula))
                
            overlap_spec(spec_list,merged_specification,template['overlap'])

            # Write expanded formulas to a file
            with open(globals.out_folder + 'specifications.txt', 'a') as file:
            #    file.write(f"Expanded formula {j} for template {i}:\n")
                file.write(f"{expanded_formula['formula']}\n")

            #update the global spec counter    
            specounter += 1

    if globals.debug:
        print(globals.CDBG+"DEBUG_MSG"+globals.CEND)
        print("Merged specification:")
        print(merged_specification)
        print("\n")
    return merged_specification, spec_list

#This function is used to overlap the specifications
#spec_list is the list of specifications
#first is the index of the first specification from this template
#Since we are using the same template we know the overlap value is the same 
def overlap_spec(spec_list,merged_specification,overlap):
    #if there is only one specification we don't need to overlap
    if len(spec_list)-1 == 0:
        return
    current_spec = spec_list[len(spec_list) - 1]
    #get current spec formula
    current_spec_overlapped = current_spec['formula']
    #get current spec inputs and outputs
    current_spec_inputs = current_spec['inputs'].split(',')
    current_spec_outputs = current_spec['outputs'].split(',')

    #Divide the overlap value by 2 to gen an equal overlap on both sides
    #Arbitrarily assign the extra overlap to the consequent side
    overlap_ant = (overlap // 2)
    overlap_con = overlap // 2+ (overlap % 2)

    #if inputs+outputs is less than the overlap value adjust overlap values to match the number of inputs and outputs
    if len(current_spec_inputs) + len(current_spec_outputs) < overlap:
        print(globals.CWRN + "Warning:" + globals.CEND + "Overlap value is greater than the number of inputs and outputs. Adjusting overlap value to match the number of inputs and outputs")
        overlap_ant = len(current_spec_inputs)
        overlap_con = len(current_spec_outputs)

    #get all inputs and outputs
    inputs_globals = merged_specification['inputs'].split(',')
    outputs_globals = merged_specification['outputs'].split(',')
    inputs_globals_copy = copy.deepcopy(inputs_globals)
    outputs_globals_copy = copy.deepcopy(outputs_globals)

    #remove current spec inputs and outputs from the merged specification
    for inp in current_spec['inputs'].split(','):
        inputs_globals.remove(inp)
    for out in current_spec['outputs'].split(','):
        outputs_globals.remove(out)

    #select overlap_ant random samples from inputs and outputs
    antecedent_prop_overlapped = random.sample(inputs_globals, overlap_ant)
    consequent_prop_overlapped = random.sample(outputs_globals, overlap_con)

    #select overlap_ant/con random samples from current spec inputs and outputs
    antecedent_prop = random.sample(current_spec_inputs, overlap_ant)
    consequent_prop = random.sample(current_spec_outputs, overlap_con)

    #substitute the selected inputs and outputs with the current spec inputs and outputs
    for i in range(0, overlap_ant):
        current_spec_inputs[current_spec_inputs.index(antecedent_prop[i])] = antecedent_prop_overlapped[i]
    for i in range(0, overlap_con):
        current_spec_outputs[current_spec_outputs.index(consequent_prop[i])] = consequent_prop_overlapped[i]
    #subtitute also in the formula
    for i in range(0, overlap_ant):
        current_spec_overlapped = current_spec_overlapped.replace(antecedent_prop[i],antecedent_prop_overlapped[i])
    for i in range(0, overlap_con):
        current_spec_overlapped = current_spec_overlapped.replace(consequent_prop[i],consequent_prop_overlapped[i])

    #Remove the subtituted inputs and outputs from the merged inputs and outputs
    for inp in antecedent_prop:
        inputs_globals_copy.remove(inp)
    for out in consequent_prop:
        outputs_globals_copy.remove(out)    



    #update the merged specification
    merged_specification['formula'] = merged_specification['formula'].replace(current_spec['formula'], current_spec_overlapped)
    merged_specification['inputs'] = ','.join(inputs_globals_copy)
    merged_specification['outputs'] = ','.join(outputs_globals_copy)

    #update the current spec inputs and outputs
    current_spec['inputs'] = ','.join(current_spec_inputs)
    current_spec['outputs'] = ','.join(current_spec_outputs)
    current_spec['formula'] = current_spec_overlapped

def main():

    print(globals.CSTP + "1." + globals.CEND + " Parsing inputs" + " \n")
    #input parameters
    parser = argparse.ArgumentParser(description='Generate circuit from LTL specifications.')
    #submodules flag
    parser.add_argument('--parallel', type=int, choices=[0, 1], required=True, help='Enable parallel module generation (1 for true, 0 for false)')
    #optional clock name 
    parser.add_argument('--clk', type=str, help='Clock signal name')
    #top module name
    parser.add_argument('--top_module', type=str, help='Top module name')
    #debug flag
    parser.add_argument('--debug', type=int, choices=[0, 1],default=0, help='Enable debug mode (1 for true, 0 for false)')
    #output directory
    parser.add_argument('--outdir', type=str, help='Output directory')
    #template string formatted in this way "{template1,nant,ncon,nspec,overlap};{template2,nant,ncon,nspec,overlap};..."
    parser.add_argument('--templates', type=str, required=True, help='List of template files')
    #trace lenght
    parser.add_argument('--tracelength', type=int, help='Trace length')

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()
    modules = args.parallel == 1
    globals.debug = args.debug == 1
    globals.clk_name = args.clk
    globals.top_module_name = args.top_module
    dirpath = args.outdir
    templates = args.templates.replace('"', '').split(';')
    globals.tracelnegth = args.tracelength
   
    template_list = []
    #parse the template string
    for template in templates:
        specification = {}
        #remove external brackets and split template into parts
        temp = template.strip()[1:-1].split(',')
        #split on commas and populate specifications fields accordingly
        specification["formula"] = temp[0]
        specification["nant"] = int(temp[1])
        specification["ncon"] = int(temp[2])
        specification["nspec"] = int(temp[3])
        specification["overlap"] = int(temp[4])
        template_list.append(copy.deepcopy(specification))

    print(globals.CSTP + "1." + globals.CEND + "     " + globals.CSTP + "Complete!" + globals.CEND + " \n")
    print(globals.CSTP + "2." + globals.CEND + " Creating LTL specifications" + " \n")
    
    #create the output directory
    dir_utils.create_outfolder()

    #create the specifications
    merged_specification, spec_list = create_specification(template_list,modules)

    print(globals.CSTP + "2."  + globals.CEND + "     " + globals.CSTP + "Complete!" + globals.CEND +  " \n")
    ###############################################################
    ############### GENERATION AND SIMULATION STAGE ###############
    ###############################################################  
    print(globals.CSTP + "3."  + globals.CEND + " Generating circuit from specification" + " \n")
    rtl_utils.generate_circuit(merged_specification,spec_list, modules)
    print(globals.CSTP + "3."  + globals.CEND + "     " + globals.CSTP + "Complete!" + globals.CEND +  " \n")
    
    print(globals.CSTP + "4." + globals.CEND + " Running HIFSuite" + " \n")
    hif_utils.run_hifsuite(merged_specification)
    print(globals.CSTP + "4."  + globals.CEND +"     " + globals.CSTP + "Complete!" + globals.CEND +  " \n")
    
    if os.path.exists(f"{globals.root}/tool/build/vcd2csv"):
        print(globals.CSTP +"4.5" + globals.CEND + " Creating CSV traces" + " \n")
        dir_utils.generateCSV()
        try:
            subprocess.run(f"mv {globals.out_folder}faulty_traces/csv/golden.csv {globals.out_folder}traces/csv/", shell=True, check=True)
            subprocess.run(f"mv {globals.out_folder}faulty_traces/vcd/golden.vcd {globals.out_folder}traces/vcd/", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to move golden traces to {globals.out_folder}/traces. {e}")

        print(globals.CSTP + "4.5"  + globals.CEND +"     " + globals.CSTP + "Complete!" + globals.CEND +  " \n")
    else:
        print(globals.CERR +"4.5" + globals.CEND + " Skipping CSV traces creation")
        print(globals.CERR +"4.5" + globals.CEND + " vcd2csv not found in tool/build directory, to generate CSV traces please compile it first" + " \n")
    
    print(globals.CSTP +"5." + globals.CEND +" Populating selected output directory" + " \n")
    dir_utils.populate_output_dir(dirpath)
    
    print(globals.CSTP + "5."  + globals.CEND +"     " + globals.CSTP + "Complete!" + globals.CEND +  " \n")
    print(globals.CSTP + "#################" + globals.CEND + f" Procedure complete! All generated files can be found in {dirpath} " + globals.CSTP + "#################"+ globals.CEND + "\n\n")

    #need to return this string for usm-t automatic test generation '{"ant":"a_0,a_1","con":"b_0,b_1"}'
    print('{\"ant\":\"' + merged_specification['inputs'] + '\",\"con\":\"' + merged_specification['outputs'] + '\"}')
    sys.exit(0)

if __name__ == "__main__":
    main()
