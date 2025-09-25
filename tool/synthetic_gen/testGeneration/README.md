# USM-T Mining challenges generator

## Description
This is a short guide on how to generate your personal mining challenges using the automatic USM-T benchmark generation pipeline

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Arguments](#Arguments)
- [Contact](#contact)

## Installation
The requirements for generating benchmarks are the same for the usage of generate_from_ltl.py (see [ltlsynt_README](../README.md)).

To correctly use the generator remember to setup your environment, you can do it using the env setup script in this repository as follows:
```bash
# Navigate to the root directory
cd path/to/usm-t_root

# set the PATH to the necessary tools
source scripts/setupEnvironment.sh
```

## Usage
To run the benchmark generation use the following command from the synthetic_gen folder: 

```bash
#launch from syntetic_gen folder
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh <ARGUMENTS>
```

## Arguments
The program will generate a all the testbench sources as well as the configuration files needed to execute a testrun. The program, if specified will also copy all the necessary files (sources and configurations) to the USM-T appropriate folders.

You can see all the cmd line options by running:

```bash
bash ~/usm-t/tool/synthetic_gen/testGeneration/testGenWrapper.sh -h
```

Here is a brief explanation of the program inputs:
- **[Optional] h**: Prints the usage message
- **syntgen_folder**: Path to the sources output folder
- **output_folder**: Path to the configurations output folder
- **test_name**:String value. Set the name of generated test in the configurations
- **top_module_name**: Specify the name for the top module of the generated design
- **clock_name**: Name of the clock signal that will be used in the generated design
- **template_string**: String that contains a list of templates representing the specifications implemented by the benchmark. More about templates [here](#about-templates)
- **reset_signal**:Name of the reset signal that will be used in the generated design
- **vcd_scope**:vcd path to the top module

- **tracelength**:Number of time instants of the generated execution traces
- **install**: Boolean value. default is 0, 1 to install, 2 to uninstall. They act as follows:
    - *Default*: Only generate design and configurations for the specified benchmark.
    - *Install*: Generate AND copy in the corresponding directories for the USM-T execution.
    - *Uninstall*: Remove the files from the USM-T folders.

The recommended approach to create your first benchmark is to copy and modify one of the generation script that you find in the [miningChallengesGenerator](../miningChallengesGenerator/) folder, e.g. [generate_sample_challgenge.sh](../miningChallengesGenerator/generate_sample_challenge.sh). 


## Contact
Main contributor: Daniele Nicoletti - daniele.nicoletti@univr.it
