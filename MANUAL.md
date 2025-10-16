# Manual: Integrating a New Miner into the USM-T Framework

## 1. Introduction

### 1.1. Purpose of the Manual

This document provides a step-by-step guide for integrating a new third-party specification mining tool (referred to as a "miner") into the Universal Specification Miner Tester (USM-T) framework.

### 1.2. Overview of the Miner Integration Process

The integration process involves the following key stages:

1.  **Setup**: Creating the necessary directory structure and configuration files for the new miner.
2.  **Data Flow Integration**: Developing "adaptors" to convert USM-T's standard data formats to and from the formats required by the new miner.
3.  **Test Case Definition**: Creating a specific test case in an XML file to run the miner with a given dataset.

### 1.3. Prerequisites

Before you begin, you must have access to the new miner you wish to integrate. This guide assumes you have one of the following:

*   A Docker image of the miner.
*   A compiled executable or source code that can be run from a shell script.

## 2. Miner Setup

This section covers the initial setup of the miner's files and default configuration within the USM-T framework.

### 2.1. Create the Miner's Directory Structure

First, create a new directory for your miner under `/miners/tools/`. The name of this directory will be used as the miner's identifier.

```bash
mkdir -p miners/tools/<your_miner_name>
```

Inside this new directory, create the following subdirectories:

*   `configurations/`: Will hold miner-specific configuration files and run scripts.
*   `docker/`: Will contain the script to launch the miner's Docker container.
*   `runs/`: This directory is used by the framework to store temporary files during test executions.

```bash
mkdir -p miners/tools/<your_miner_name>/configurations
mkdir -p miners/tools/<your_miner_name>/docker
mkdir -p miners/tools/<your_miner_name>/runs
```

### 2.2. Add the Miner's Executable/Container

The USM-T framework is designed to run miners within Docker containers. You should create a `run_docker_container.sh` script in the `miners/tools/<your_miner_name>/docker/` directory. This script is responsible for launching the miner. You can copy and adapt an existing script from another miner, such as `miners/tools/harm/docker/run_docker_container.sh`.

### 2.3. Configure the Miner in the Main Application

You must define a default "use case" for your miner. This tells the USM-T framework how to run your miner, what kind of data it expects, and how to handle its output.

Open the file `miners/usmt_default_config.xml` and add a new `<usecase>` block for your miner. You can use an existing entry as a template.

**Example Template:**

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
*   `<configuration type="support" path="...">`: (Optional) Use this for any additional configuration files your miner needs. These files will be copied into the container.
*   `<input_adaptor path="...">`: A script that converts the standard test input into the format your miner requires. See Chapter 3 for details. The path is relative to `miners/adaptors/input_adaptors/`.
*   `<output_adaptor path="...">`: A script that converts your miner's output into the standard SpotLTL format used by the framework. See Chapter 3 for details. The path is relative to `miners/adaptors/output_adaptors/`.
*   `<export ...>`: (Optional) Define environment variables that will be passed to the Docker container during execution.
*   `<docker image="...">`: The name of the Docker image for your miner.

Fill in these fields for your new miner. For the adaptors, you can start by pointing to existing ones if the data format is compatible. In the next chapter, we will cover how to create new ones if needed.

## 3. Data Flow Integration (Adaptors)

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

The output adaptor converts the specification file generated by your miner into the framework's standard `.spotltl` format. This is crucial for enabling the comparison and analysis features.

1.  **Create a directory** for your adaptor under `miners/adaptors/output_adaptors/`. The name should be descriptive, e.g., `<your_format>_to_spotltl`.

    ```bash
    mkdir -p miners/adaptors/output_adaptors/<your_format>_to_spotltl
    ```

2.  **Create a `run.sh` script** inside this directory. This script will be called with two arguments:
    *   `$1`: The path to the single output file generated by the miner.
    *   `$2`: The path where the final `.spotltl` file should be written.

3.  **Implement the script**. The script should read the miner's output file, parse it, and write the formulas in SpotLTL format to the destination file.

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

After creating your adaptors, remember to update the `<input_adaptor>` and `<output_adaptor>` paths in your miner's default configuration in `miners/usmt_default_config.xml`.

## 4. Defining a Test Case

Once your miner is configured and has the necessary adaptors, you can define a specific test case to run it.

### 4.1. Anatomy of a Test XML File

Test cases are defined in XML files located in the `tests/` directory. These files orchestrate a complete test run, specifying the input data, the miners to run, and the evaluations to perform. A test file has three main sections:

1.  **`<input>`**: Defines a set of source data (traces and designs). Paths are relative to the `input/` directory.
2.  **`<usecase>`**: Configures a specific run of a single miner for this test. It customizes the miner's default settings for a particular dataset.
3.  **`<test>`**: The main block that lists which use cases to execute and which comparison strategies to apply against an expected "golden" result.

### 4.2. Creating a New Test File

The easiest way to create a new test is to copy an existing one and modify it.

1.  **Copy an existing test file**, for example `tests/arb2.xml`, to a new file in the same directory, e.g., `tests/my_new_test.xml`.

2.  **Modify the `<input>` block** to point to the traces and/or design files for your new test. You can define multiple `<input>` blocks with different `id` attributes.

    ```xml
    <input id="0">
      <csv path="my_test_data/traces/my_golden.csv"/>
      <verilog path="my_test_data/design/my_design.v"/>
    </input>
    ```

3.  **Define a `<usecase>` for your miner**. Give it a unique `id`. This block tells the framework exactly how to run your miner for this specific test.

    ```xml
    <usecase id="my_miner_run_1">
      <miner name="<your_miner_name>"/> 
      <input id="0" type="csv" dest_dir="my_input_dir/"/>
      <configuration type="run" path="my_test_ex/run_miner.sh"/>
      <configuration type="support" path="my_test_ex/my_config.txt"/>
      <export MY_VAR="some_value_for_this_test"/>
    </usecase>
    ```
    *   **`<miner name>`**: The name of your miner.
    *   **`<input id="..." type="..." dest_dir="..."/>`**: Selects the input data (using the `id` from the `<input>` block) and specifies which `type` of data to use. `dest_dir` is the directory where the input files will be placed inside the miner's container.
    *   **`<configuration path="..."/>`**: Points to test-specific run scripts or support files located in `miners/tools/<your_miner_name>/configurations/`.
    *   **`<export ...>`**: Defines environment variables specifically for this test run, which can be used by your `run_miner.sh` script.

4.  **Add your use case to the `<test>` block**. In the `<test>` block at the end of the file, add a `<usecase>` tag referencing the `id` of the use case you just defined.

    ```xml
    <test name="my_new_test_all_goals">
      <!-- Add your use case here -->
      <usecase id="my_miner_run_1"/>

      <!-- (Optional) Add other miners to compare against -->
      <usecase id="harm_usecase_1_bool"/>

      <expected path="input/my_test_data/expected/golden_formulas.txt"/>

      <compare with_strategy="n_mined"/>
      <compare with_strategy="syntactic_similarity"/>
      <compare with_strategy="time_to_mine"/>
    </test>
    ```

5.  **Update the `<expected>` path** to point to the file containing the golden specifications for your test data. This is necessary for the comparison strategies to work.
