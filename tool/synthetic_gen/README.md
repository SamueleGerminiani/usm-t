# Syntetic generation of verilog designs

## Description
This project allows to generate automatically a verilog implementation starting from LTL specifications. Together with the Verilog design, the execution traces and design specification are generated.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Arguments](#Arguments)
- [Contact](#contact)

## Installation
The requirements for the usage of generate_from_ltl.py are the following:    
- Python (at least 3.10)
- ltlsynt (see ../third_party/install_ltlsynt.sh)
- yosys (see ../third_party/install_os-cad.sh)
- hif_suite (see ../third_party/install_hifsuite.sh)

To use correctly the gen script ltlsynt, yosys and hif binaries need to be added to the $PATH, you can do it using the env setup script in this repository as follows:
```bash
# Navigate to the root directory
cd path/to/usm-t_root

# set the PATH to the necessary tools
source scripts/setupEnvironment.sh
```

**[OPTIONAL] CSV TRACES GENERATION**

It is possible to also generate execution traces in the .csv format (default is only .vcd traces).
To enable this functionality "vcd2csv" needs to be compiled using the USM-T compilation procedure (see ../../README.md)

## Usage
To run the generation of verilog designs simply use the following command from the "tool/syntetic_gen" folder: 

```bash
# Run the project
python generate_from_ltl.py config_file.xml
```

## Arguments
The program generates a verilog design based on templates provided in input.

You can see all the cmd line options by running:

```bash
python generate_from_ltl -h
```

Here is a brief explanation of the program inputs:
- **[Optional] h**: Prints the terminal help message
- **parallel**: boolean value (0 or 1). If set to 1 each specification will be used to generate a standalone module. If set to 0 a single monolitic module will be generated
- **[Optional] clk**: String that can be used to specify a custom clock name 
- **debug**: bolean value. If 1 enable debug features
- **[Optional] outdir**: path to the directory for the generated files. If not specified the default directory is ".synthetic_gen_output"
- **templates**: String that contains a list of templates

    ## About templates: 
    A list of templates that can be specified by the user as a string. Each template need represents the LTL formula structure. A template is accepted by the tool if it is accepted by the ltlsynt grammar (see https://spot.lre.epita.fr/ltlsynt.html).
    The accepted format for the template string is as follows: 
    ```bash
    {template1,nant,ncon,nspec,overlap},{template2,nant,ncon,nspec,overlap},...
    ```
    Where:
    - **nant**: number of proposition that will be used for expanding the antecedent
    - **ncon**: number of propositions that will be used for expanding the consequent
    - **nspec**: number of specification that will be generated from this template
    - **overlap**: number of proposition that will be overlapped with other specifications

    The templates can be specified using special placeholders (**..&&..**, **..##N..** and **..#1&..**) taht will be expanded as follows:
    -  **..&&..** : prop0 & prop1 & prop2 & ...
    -  **..##N..** : prop0 ##N prop1 ##N prop2 ##N ...
    -  **..#N&..** : prop0 & prop1 ##N prop2 & ...
    
    <sub><sup>Where "N" is an integer specified by the user</sup></sub>

## Contact
Main contributor: Daniele Nicoletti - daniele.nicoletti@univr.it
