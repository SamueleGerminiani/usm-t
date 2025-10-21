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
To generate a benchmark, you can run one of the provided scripts in this directory. For example, to generate the 'And' challenge, run:
```bash
bash generate_And.sh [install] [debug]
```
The `install` and `debug` arguments are optional.

The scripts in this directory call the main wrapper script `tool/synthetic_gen/testGenerator/wrapper.sh` with pre-filled parameters for each challenge.

## Arguments
The generation scripts in this directory accept two optional positional arguments:
1.  `install`: (Optional) Integer value (0, 1, or 2). Default is 0.
    - `0`: Only generate the challenge in the `raw_challenges` directory.
    - `1`: Generate and install the challenge into the USM-T framework.
    - `2`: Uninstall the challenge from the USM-T framework.
2.  `debug`: (Optional) Integer value (0 or 1). Default is 0. `1` enables debug mode for more verbose output.

The internal parameters of each challenge (e.g., number of specifications, LTL formula template) are defined within each `generate_*.sh` script. If you want to customize a challenge, it is recommended to copy an existing script and modify these parameters.

The recommended approach to create your first benchmark is to copy and modify one of the generation scripts that you find in this folder, e.g. `generate_And.sh`.


## Contact
Main contributor: Daniele Nicoletti - daniele.nicoletti@univr.it
