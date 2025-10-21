# Manual: Integrating a New Miner into the USM-T Framework

## 1. Introduction

### 1.1. Purpose of the Manual

This document provides a step-by-step guide for integrating a new third-party specification mining tool (referred to as a "miner") into the Universal Specification Miner Tester (USM-T) framework.

### 1.2. Overview of the Miner Integration Process

The integration process involves the following key stages:

1.  **Setup**: Creating the necessary directory structure and configuration files for the new miner.
2.  **Data Flow Integration**: Developing "adaptors" to convert USM-T's standard data formats to and from the formats required by the new miner.
3.  **Test Case Definition**: Creating a specific test case in an XML file to run the miner with a given dataset. Or adding a new miner in an existing text.

### 1.3. Prerequisites

Before you begin, you must have access to the new miner you wish to integrate. This guide assumes that the miner is available as a Docker image.

## 2. Miner Setup

This section covers the initial setup of the miner's files and default configuration within the USM-T framework.

### 2.1. Create the Miner's Directory Structure

You can create the necessary directory structure manually or by using the provided `scripts/createMiner.sh <miner_name>` script. This script will generate a basic template for your miner, which you can then customize.

First, create a new directory for your miner under `/miners/tools/`. The name of this directory will be used as the miner's identifier.

```bash
mkdir -p miners/tools/<your_miner_name>
```

Inside this new directory, create the following subdirectories:

*   `configurations/`: Will hold miner-specific configuration files and run scripts. While not mandatory, the best practice is to create a subdirectory here for each distinct configuration or test case (e.g., `arb2_ex/`).
*   `docker/`: This directory is not mandatory, but can be used to store utility files. For example, you can add a `download_image.sh` script to pull the miner's Docker image. If each miner has such a script, you can run `scripts/downloadMiners.sh` to download all miner images at once.
*   `runs/`: This directory serves as a repository for each individual miner use case run, containing all the necessary information for debugging. A new directory is created for each run, named according to the convention `<test_name>_<usecase_id>_<timestamp>`. Additionally, a symbolic link named `latest` always points to the most recent run directory. Inside each run directory, you will find the following:
    *   `adapted/`: Contains the final specification files after being processed by the output adaptor.
    *   `config_standalone/`: A directory containing a `config.xml` file. This XML is a complete USM-T test configuration containing only the test for the current miner-usecase, allowing for quick, isolated re-runs.
    *   `evaluation/`: Contains evaluation reports, such as fault coverage or similarity scores, for this specific use case.
    *   `input/`: Contains the input files after being processed by the input adaptor.
    *   `original_input/`: Contains the original, unmodified input files for the test.
    *   `output/`: Contains the raw, unmodified output from the miner before being processed by the output adaptor.

```bash
mkdir -p miners/tools/<your_miner_name>/configurations
mkdir -p miners/tools/<your_miner_name>/docker
mkdir -p miners/tools/<your_miner_name>/runs
```

### 2.2. Add the Miner's Executable/Container

The USM-T framework runs miners within Docker containers using a generic `run_docker_container.sh` script. This script is not meant to be modified by the user. Instead, you must configure the miner's execution by defining properties in a test-specific XML file located in the `tests/` directory (e.g., `tests/your_test.xml`).

This generic script handles the Docker container lifecycle and uses the following XML configurations:

*   **Docker Image**: The script uses the image specified in the `<docker image="your/docker_image:latest"/>` tag.
*   **Run Command**: The command to execute the miner is specified via the `<configuration type="run" path="your_script.sh"/>` tag. The `path` points to a script located in `miners/tools/<your_miner_name>/configurations/`. This script will be executed inside the Docker container.

The generic script sets up the container environment as follows:
*   Input files (traces, designs) are mounted at the `/input` directory inside the container. Your `run_miner.sh` script should expect the input data to be in this directory.
*   The framework expects the miner to write its output files to the `/output` directory. This output will then be processed by the specified output adaptor.
*   Any variables defined with the `<export .../>` tag are passed as environment variables to the container, which can be used within your `run_miner.sh` script.

### 2.3. Define a Miner Usecase

To run your miner, you need to define a `<usecase>` for it within a test XML file located in the `tests/` directory. This XML file is the central point for configuring a test run.

A `<usecase>` block specifies how the framework should execute your miner, what data to provide, and how to handle its output.

**Example Usecase:**

```xml
<usecase id="<your_miner_name>_default">
  <miner name="<your_miner_name>"/>
  <input id="0" type="csv"/>
  <configuration type="run" path="<your_miner_name>/run_miner.sh"/>
  <input_adaptor path="csv_to_csv/run.sh"/>
  <output_adaptor path="spotltl_to_spotltl/run.sh"/>
  <export MY_VAR="some_value"/>
  <docker image="your/docker_image:latest"/>  
</usecase>
```

**Explanation of the tags:**

*   `<miner name="...">`: The name of your miner. This **must** match the directory name you created in step 2.1.
*   `<input type="...">`: The default input data type your miner consumes (e.g., `vcd`, `csv`, `verilog`). You can list multiple types separated by commas.
*   `<configuration type="run" path="...">`: The path to the main script that executes the miner. This path is relative to the `miners/tools/<your_miner_name>/configurations/` directory.
*   `<configuration type="support" path="...">`: (Optional) Use this for any additional configuration files your miner needs, such as its own XML configuration file. These files will be copied into the container and should be located in `miners/tools/<your_miner_name>/configurations/`.
*   `<input_adaptor path="...">`: A script that converts the standard test input into the format your miner requires. See Chapter 3 for details. The path is relative to `miners/adaptors/input_adaptors/`.
*   `<output_adaptor path="...">`: A script that converts your miner's output into the standard SpotLTL format used by the framework. See Chapter 3 for details. The path is relative to `miners/adaptors/output_adaptors/`.
*   `<export ...>`: (Optional) Define environment variables that will be passed to the Docker container during execution.
*   `<docker image="...">`: The name of the Docker image for your miner.

Fill in these fields for your new miner. For the adaptors, you can start by pointing to existing ones if the data format is compatible. In the next chapter, we will cover how to create new ones if needed.

## 3. Data Flow Integration (Adaptors)

> **Remark:** It is unlikely that you will need to create a new adaptor. The USM-T framework provides adaptors for many common data formats. If you find that you need to create a new one, it may indicate that the miner you are integrating does not adhere to widely used standards for trace or specification formats.

Adaptors are scripts that act as a bridge between the USM-T's standard data formats and the specific formats your miner uses. If your miner does not natively support the framework's VCD, CSV, or Verilog formats, you will need to create one or both of the following adaptors.

### 3.1. Input Adaptor

The input adaptor converts trace files from the framework's format to the format your miner expects.

1.  **Create a directory** for your new adaptor under `miners/adaptors/input_adaptors/`. The name should be descriptive, e.g., `csv_to_<your_format>`.

    ```bash
    mkdir -p miners/adaptors/input_adaptors/csv_to_<your_format>
    ```

2.  **Create a `run.sh` script** inside this new directory. This script will be called with two arguments:
    *   `$1`: The path to a directory containing the input files (e.g., `.csv` or `.vcd` files).
    *   `$2`: The path to an output directory where the converted files should be written.

3.  **Implement the script**. The script should loop through the files in the input directory, convert each one, and save the result in the output directory.

**Example `run.sh` for an input adaptor:**

```bash
#!/bin/bash

# $1: Input directory
# $2: Output directory

# Ensure the output directory exists
mkdir -p "$2"

# Loop through all files in the input directory
for f in "$1"/*
do
  echo "Converting $(basename "$f")"
  # Your conversion logic here. For example, using a Python script:
  # python3 /path/to/your/converter.py "$f" > "$2/$(basename "$f").<new_extension>"
done

echo "Input conversion complete."
```

### 3.2. Output Adaptor

The output adaptor converts the specification file generated by your miner into one of the framework's standard formats, such as SpotLTL, classic PSL, or SystemVerilog assertions. This is crucial for enabling the comparison and analysis features.

1.  **Create a directory** for your adaptor under `miners/adaptors/output_adaptors/`. The name should be descriptive, e.g., `<your_format>_to_spotltl`.

    ```bash
    mkdir -p miners/adaptors/output_adaptors/<your_format>_to_spotltl
    ```

2.  **Create a `run.sh` script** inside this directory. This script will be called with two arguments:
    *   `$1`: The path to the single output file generated by the miner.
    *   `$2`: The path to the output file where the converted specifications should be written.

3.  **Implement the script**. The script should read the miner's output file, parse it, and write the resulting formulas to the destination file specified by `$2`. The framework expects the output file to contain one specification per line.

**Example `run.sh` for an output adaptor:**

```bash
#!/bin/bash

# $1: Input file from miner
# $2: Output .spotltl file path

echo "Converting miner output to SpotLTL"

# Your conversion logic here.
# For example, if the miner output is a text file with one formula per line:
# cat "$1" | while read -r line ; do
#   # (Example) Convert formula syntax if needed
#   echo "$line" >> "$2"
# done

# If no conversion is needed and the output is already in spotltl format:
cp "$1" "$2"

echo "Output conversion complete."
```

After creating your adaptors, remember to update the `<input_adaptor>` and `<output_adaptor>` paths in the appropriate `<usecase>` within your test XML file.

## 4. Defining a Test Case (Test XML File)

In the previous chapter, we introduced adaptors as a mechanism to connect miners with the USM-T data flow. We now move from data transformation to test orchestration. A **test** in USM-T is defined by an XML file that specifies:
- which input data should be used,
- which miners should be executed,
- and how their results should be evaluated.

This chapter explains the structure of a test XML file and how to write one from scratch. Understanding test XML files is essential because each new miner must be validated within at least one test case to be executed by the framework.

---

### 4.1 Overview of Test XML Files

A test XML file is located in the `tests/` directory of the project and has the following general layout:

```xml
<usmt>
  <input>...</input>
  <usecase>...</usecase>
  <test>...</test>
</usmt>
```

Each section plays a specific role:
- `<input>` defines where the test gets its data from,
- `<usecase>` defines how a miner is run with this data,
- `<test>` groups usecases and applies evaluation strategies.

The following sections explain each part in the required order.

---

### 4.2 The `<input>` Section

The `<input>` section defines the datasets that will be used in the test. These datasets can include execution traces, hardware designs, and optional auxiliary files. 
A test file can include multiple `<input>` blocks, each with a unique `id`.

Example:
```xml
  <input id="0">
    <vcd path="arb2/traces/bool/golden.vcd" clk="clk" rst="rst" scope="arb2_bench::arb2_"/>
    <csv path="arb2/traces/bool/golden.csv"/>
    <verilog path="arb2/design_bool/arb2_bool.v"/>
  </input>
```

- The paths a relative to $USMT_ROOT/input/
- Each dataset is assigned to an `id` so that it can be referenced later by one or more `<usecase>` sections.
- If both csv and vcd traces are used, the resulting vcd set must be equal to the csv trace and viceversa, both from a naming point of view of the single elements (with different suffix) and in the content of the trace
- Source designs of different types (e.g. Verilog vs C++) are not checked for equivalence
- clk and scope are mandatory when using vcd, the scope is the hierachical path from the top module separated with ::

Also directories containing the traces and the design can be defined (the suffix of the files must match the type, e.g. .vcd for VCD, .csv for CSV, .v for Verilog)

Example:
```xml
  <input id="1">
    <vcd_dir path="arb2/traces/bool/" clk="clk" rst="rst" scope="arb2_bench::arb2_"/>
    <csv_dir path="arb2/traces/bool/"/>
    <verilog_dir path="arb2/design_bool/"/>
  </input>
```



---

### 4.3 The `<usecase>` Section

The `<usecase>` section defines how a single miner is executed within the test. It specifies:
- which input dataset to use,
- which adaptors are needed,
- the miner container to run,
- and any additional settings needed for that execution.

A `<usecase>` must have a unique identifier so it can later be referenced in the `<test>` section.

Example:
```xml
  <usecase id="harm_usecase_1_bool">
    <miner name="harm"/> 
    <input id="0" type="vcd" dest_dir="bool/"/>
    <configuration type="support" path="arb2_ex_bool/generic.xml"/>
    <configuration type="run" path="arb2_ex_bool/run_miner.sh"/>
    <input_adaptor path="vcd_to_vcd/run.sh"/>
    <output_adaptor path="spotltl_to_spotltl/run.sh"/>
    <export MAX_THREADS="1" CONFIG="generic.xml" VCD_DIR="bool/" CLK="clk" VCD_SS="arb2_bench::arb2_"/>
    <docker image="samger/harm:latest"/>  
  </usecase>
```

Explanation:
- `<miner name="<miner_name>"/>` selects the miner by folder name in `$USMT_ROOT/miners/tools/<miner_name>/`.
- `<input id="..." type="..." dest_dir="<target_dir>/">` connects this usecase to the input defined earlier. The `type` attribute specifies which input types to use (comma-separated if multiple, e.g.`vcd, verilog`). The `dest_dir` is the directory (it will create that directory) where the miner will write the input files relative to `/input/`, in the docker container).
.
- `<configuration type="..." path="..."/>` defines the configuration files to use for this usecase. The path is relative to `$USMT_ROOT/miners/<miner_name>/configurations/`. If type="run", then it must points to the bash script used to run the miner. If type="support", then it specifies additional configuration files that are needed by the miner and will be copied to `/input/` in the docker container. If type="support", it specifies an additional configuration file that is needed by the miner and will be copied to /input/ in the docker container.
- `<input_adaptor path="...">` and `<output_adaptor path="...">` scripts handle data conversion, the path is relative to `$USMT_ROOT/miners/adaptors/input_adaptors/` and `$USMT_ROOT/miners/adaptors/output_adaptors/`, respectively.
- `<docker image="..."/>` specifies which Docker image contains the miner.
- `<export VAR_NAME="..." />` optionally defines environment variables for that miner.

A test XML file may define multiple usecases to run different miners or configurations on the same data.

---

### 4.4 The `<test>` Section

The `<test>` section brings everything together. It specifies which usecases will be executed and which evaluation strategies will be applied to compare the miners’ results.
The `<test>` section is required for every test XML file and is executed when running the framework.

```xml
  <test name="arb2_all_goals">
    <usecase id="harm_usecase_1_bool"/>
    <usecase id="goldminer_usecase_1"/>
    <usecase id="texada_usecase_1"/> 
    <usecase id="samples2ltl_usecase_1"/>
    <external id="manually_defined"/>

    <expected path="input/arb2/expected/arb2_golden_ass_bool.txt"/>

    <compare with_strategy="n_mined"/>
    <compare with_strategy="syntactic_similarity"/>
    <compare with_strategy="semantic_equivalence"/>
    <compare with_strategy="hybrid_similarity"/>
    <compare with_strategy="fault_coverage" faulty_traces="input/arb2/faulty_traces/csv/" trace_type="csv" />
    <compare with_strategy="time_to_mine"/>
  </test>
```

Explaination:
- The `name` attribute defines an identifier for the test run.
- One or more `<usecase>` tags specify which miners will participate.
- The `<expected>` tag provides a file containing the **golden set of specifications** (one for each line) used for comparison. The path is relative to `$USMT_ROOT/`. This is neccessary for all comparison strategies except for time_to_mine, fault_coverage and n_mined.

- Each `<compare>` tag selects an evaluation strategy witht the `with_strategy` attribute.

Typical evaluation strategies include:
- `n_mined`: counts the number of mined specifications.
- `syntactic_similarity`: compares mined formulas textually.
- `semantic_equivalence`: checks logical equivalence.
- `hybrid_similarity`: combines syntactic and semantic measures.
- `fault_coverage`: evaluates detection capability using faulty traces, the `faulty_traces` attribute specifies the path to the directory containing the faulty traces (relative to $USMT_ROOT/), and trace_type specifies the type of the traces (csv or vcd). The `trace_type` specifies the type of the traces (csv or vcd).
- `time_to_mine`: measures mining time performance.


---

