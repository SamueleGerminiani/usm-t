# Syntetic generation of verilog designs

## Description
This project allows to generate automatically a verilog implementation starting from LTL specifications. Together with the Verilog design, the execution traces and design specification are provided.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Installation
The requirements for the usage of gen.py are the following:
    - Python (at least 3.10)
    - ltlsynt (see ../third_party/spot_install.sh)
    - yosys (see ../third_party/install_os-cad.sh)
    - verilator (at least 4.210)

To use correctly the gen script ltlsynt and yosys binaries need to be added to the $PATH, you can do it using the env setup script in this repository as follows:
```bash
# Navigate to the root directory
cd path/to/usm-t

# set the PATH to the necessary tools
source env-setup.sh

```

## Usage
To run the generation of verilog designs simply use the following command from the "tool/syntetic_gen" folder: 

```bash
# Run the project
python gen.py
```

## Expanding the template library

The program generates a verilog design based on templates contained in the specs/output_templates.xml file. If you wish to add a new template to the generation script you can use the "template_lib.py" in syntetic_gen/scripts folder by running:

```bash
#Generate the template library
python template_lib.py input_file.txt output_file.xml
```

The programs take as input a .txt file in which a list of templates with corresponding input and output signals are specified and converts it into an xml representation that is accepted by the gen.py program.

Here is an example of an accepted input file:

```
#1. Sequential Behavior
Input : a
Output : c
Template: G(..##1.. |=> ..##1..)

#2. 
#Input : a
#Output : c
#Template: G(..&&.. |-> ..&&..)

#3. Always Eventually with Compound Antecedents
Input : ant_1, ant_2
Output : cons
Template: G((ant_1 & ant_2) -> F cons)
```

Note that, special operations as **..&&..** and **..##1..** will be expanded by the gen.py program as:
    -  **..&&..**: prop0 & prop1 & prop2 & ...
    -  **..##1..**: prop0 ##1 prop1 ##1 prop2 ##1 ...

## Contact
Daniele Nicoletti - daniele.nicoletti@univr.it