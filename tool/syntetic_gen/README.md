# Syntetic generation of verilog designs

## Description
This project allows to generate automatically a verilog implementation starting from LTL specifications. Together with the Verilog design, the execution traces and design specification are generated.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Contributing](#contributing)
- [Contact](#contact)

## Installation
The requirements for the usage of gen.py are the following:
    - Python (at least 3.10)
    - ltlsynt (see ../third_party/spot_install.sh)
    - yosys (see ../third_party/install_os-cad.sh)
    - hif_suite (see ../third_party/install_hifsuite.sh)

To use correctly the gen script ltlsynt, yosys and hif binaries need to be added to the $PATH, you can do it using the env setup script in this repository as follows:
```bash
# Navigate to the root directory
cd path/to/usm-t_root

# set the PATH to the necessary tools
source env-setup.sh

```

## Usage
To run the generation of verilog designs simply use the following command from the "tool/syntetic_gen" folder: 

```bash
# Run the project
python gen.py config_file.xml
```

## Expanding the template library

The program generates a verilog design based on templates contained in the config.xml file provided as input. If you wish to add a new template to the generation script you can modify the configuration file as you please.

The configuration file provides as the parameters necessary for the automatic syntesys and simulation of a design.
Here is an example of an accepted input file:

```xml
<sygen>
    <template ins="a" outs="c" text="G(..##1.. |=&gt; ..##1..)"/>
    <template ins="a" outs="c" text="G(..&amp;&amp;.. |=&gt; ..&amp;&amp;..)"/>
    <template ins="ant" outs="con" text="G( ant |=&gt; con)"/>
    
    <parameter ntemplates="2" nant="2" ncon="1" nspec="3" parallel='1' />
</sygen>

```

The configuration file contains two main elements:

1. A list of templates that can be extended indefinitely by the user. Each template need to contain a list of input and output signals and the LTL formula structure. A template is accepted by the tool if it is accepted by the ltlsynt grammar (see https://spot.lre.epita.fr/ltlsynt.html).

    Note that, special operations as **..&&..** and **..##1..** will be expanded by the gen.py program as:
    -  **..&&..** : prop0 & prop1 & prop2 & ...
    -  **..##1..** : prop0 ##1 prop1 ##1 prop2 ##1 ...

2. A unique definition of parameters to configure the generation:
    - **ntemplates**:number of templates that will be used in the **generation**. If this value is less that the number of specified templates, they will be chosen randomly from the available ones 
    - **nant**: number of proposition that will be used for expanding the antecedent
    - **ncon**: number of propositions that will be used for expanding the consequent
    - **nspec**: number of specification that will be used to generate the design
    - **parallel**: boolean value (0 or 1). If set to 1 each specification will be used to generate a standalone module. If set to 0 a single monolitic module will be generated



## Contact
Main contributor: Daniele Nicoletti - daniele.nicoletti@univr.it