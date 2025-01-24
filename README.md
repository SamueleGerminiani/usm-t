
# The official repository of the Universal Specification Miner Tester (USM-T) tool

## Table of contents

[Project info](#project-info)

[Quick start](#quick-start)
1. [Install dependencies](#dependencies)
2. [Build the project](#build-the-project)
3. [Run default tests](#run-default-tests)

[How to use the tool](#how-to-use-the-tool)

[The configuration file](#the-configuration-file)

[Optional arguments](#optional-arguments)

[Citations](#citations)

## Project info

This project introduces a robust framework designed for evaluating and comparing LTL (Linear Temporal Logic) specification miners. Traditional approaches often struggle with subjective assessments and intricate configurations; our solution addresses these issues by offering a structured methodology to assess the quality of specifications through both semantic and syntactic analyses.

## Quick start

Currently, we support only Linux with gcc and clang (C++17) and cmake 3.14+.

### Dependencies
* [spotLTL](https://spot.lrde.epita.fr/install.html)
* [antlr4-runtime](https://www.antlr.org)
* [gedlib](https://dbblumenthal.github.io/gedlib/)
* [z3](https://github.com/Z3Prover/z3.git)

#### Ubuntu

```
sudo apt-get install -y uuid-dev pkg-config
```

#### Third party

* Install all dependencies. These will be compiled from source but will not affect your system, as all dependencies are installed in the third_party directory.

```
cd third_party
bash install_all.sh
```

## Build the project

Create a build directory inside the tool directory

```
cd tool
mkdir build && cd build
```

Configure the build environment with cmake.

For a build without SpotLTL (the generated binaries will comply with the MIT license), and the evaluation of temporal formulas will follow a modular-based approach:

```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Compile the project:

```
make
```

## How to use the tool

Set the environment variables for the tool:

```
bash scripts/setupEnvironment.sh
```

Download the miners:

```
bash scripts/downloadMiners.sh
```

## How to run the tool
./usm-t --test <path_to_config_file>

## The configuration file

The configuration file in XML format specifies inputs and use cases for testing with the Universal Specification Miner Tester. Here is a complete example of the configuration file, followed by an in-depth explanation of each element:

```xml
  <input id="0">
    <vcd path="arb2/traces/bool/golden.vcd" clk="clk" rst="rst" scope="arb2_bench::arb2_"/>
    <csv path="arb2/traces/bool/golden.csv"/>
    <verilog path="arb2/design_bool/arb2_bool.v"/>
  </input>
```

- Defines the input of the test: traces and/or design
- The paths a relative to $USMT_ROOT/input/
- The id can be used in usecases to use this test
- If both csv and vcd traces are used, the resulting vcd set must be equal to the csv test and viceversa, both from a naming point of view of the single elements (with different suffix) and in the content of the trace
- Source designs are not checked for equivalence
- clk and scope are mandatory, the scope is the hierachical path from the top module separate with ::

```xml
  <input id="1">
    <vcd_dir path="arb2/traces/bool/" clk="clk" rst="rst" scope="arb2_bench::arb2_"/>
    <csv_dir path="arb2/traces/bool/"/>
    <verilog_dir path="arb2/design_bool/"/>
  </input>
```

- Also directories containing the traces and the design can be defined (suffix must match the type, e.g. .vcd for VCD, .csv for CSV, .v for Verilog)

```xml
  <usecase id="harm_usecase_1_bool">
    <miner name="harm"/> 
    <input id="0" type="vcd" dest_dir="bool/"/>
```

- Select the input to use for this usecase specifying the id and the types (as a comma separated list) of the input to use
- dest_dir is the directory where the miner will write the input files relative to /input/ (of the docker container)


```xml
    <config type="support" path="config1_bool/generic.xml"/>
    <config type="run" path="config1_bool/run_miner.sh"/>
```

- Define the configuration files to use for this usecase. The type can be either "support" or "run". "run" specifies a path to the file containing an sh script to run the miner. The script will be used in the run_container.sh script (in $USMT_ROOT/miners/<miner_name>/docker/) to run the miner.
- "support" specifies additional configuration files that are needed by the miner. 
- The path is relative to $USMT_ROOT/miners/<miner_name>/configurations/


```xml
    <input_adaptor path="vcd_to_vcd/run.sh"/>
    <output_adaptor path="spotltl_to_spotltl/run.sh"/>
```
- Define the adaptors to use for this usecase.
- Each input adaptor must have two inputs, a path to a directory containing the input files and a path to a directory where the output files will be written.
- Each output adaptor must have two inputs, a path to a file containing the mined assertions in the custom format of the miner, and a path to an output file where the assertions will be written in a supported format (e.g. spotltl).

```xml
<export MAX_THREADS="1" CONFIG="generic.xml" VCD_DIR="bool/" CLK="clk" VCD_SS="arb2_bench::arb2_"/>
  </usecase>
```

- Variables exported here can be used in the script to run the miner. The variables are passed as environment variables to the docker container running the miner.


```xml
<test name="arb2_test_1">
```
- Define the test to run

```xml
    <usecase id="samples2ltl_usecase_1"/>
    <usecase id="harm_usecase_1_bool"/>
    <usecase id="goldminer_usecase_1"/>
    <usecase id="texada_usecase_1"/> 
```

- Define the usecases to run for this test (must be defined above)
- They will appear in the summary report in the same order as in this xml


```xml
<expected path="input/arb2/expected/arb2_golden_ass_bool.txt"/>
```
- File containing the golden assertions to compare with
- The path is relative to the root of usm-t
- This is neccessary for all comparison strategies except for time_to_mine and fault_coverage

```xml
    <compare with_strategy="n_mined"/>
    <compare with_strategy="syntactic_similarity"/>
    <compare with_strategy="semantic_equivalence"/>
    <compare with_strategy="hybrid_similarity"/>
    <compare with_strategy="fault_coverage" faulty_traces="input/arb2/faulty_traces/" trace_type="csv" />
    <compare with_strategy="time_to_mine"/>
  </test>
</usm-t>
```
- List of evaluation strategies to use
- They will appear in the summary report in the same order as in this xml

## Optional arguments

List any optional command-line arguments and their descriptions.

## Citations

If you need to reference this tool in an academic publication, please use the following citation (add details here):

\```bibtex
@article{YourCitation,
  title={Title},
  author={Author},
  journal={Journal},
  year={Year},
  volume={Volume},
  pages={Pages}
}
\```

