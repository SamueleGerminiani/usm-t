# Synthetic Challenge Generation

This document explains how to use the synthetic challenge generation tools.

The scripts generate a comprehensive challenge package that includes:

*   **Hardware Design:** Verilog source code for the hardware module.
*   **Execution Traces:** Sample execution traces in both CSV and VCD formats, showing the behavior of the design.
*   **Golden Specifications:** The formal properties (in LTL) that the design is expected to satisfy. These are the "answers" that the mining tools should find.
*   **Default Miner Configurations:** Pre-made configuration files for several specification mining tools:
    *   harm
    *   goldminer
    *   texada
    *   samples2ltl
    *   gemini

    These configurations provide the miners with basic information about the design, such as its input/output signals and the desired depth of analysis for mining behaviors.

## Challenge Generators

The `challengeGenerators` directory contains scripts to generate different types of challenges. These challenges are based on different LTL formulas.

### Usage

To generate a challenge, run the corresponding script from the `challengeGenerators` directory.

```bash
bash challengeGenerators/generate_<type>.sh [install] [debug]
```

**Arguments:**

*   `install`: (Optional) An integer to control installation.
    *   `0` (default): Generate the challenge but do not install it.
    *   `1`: Generate and install the challenge. This will place all the generated files in the correct directories within the project, so the user only needs to run the mining tool with the main XML configuration file.
    *   `2`: Uninstall the challenge.
*   `debug`: (Optional) An integer to control debug mode.
    *   `0` (default): Run in normal mode.
    *   `1`: Run in debug mode, which may produce more verbose output.

**Prerequisites:**

*   The `USMT_ROOT` environment variable must be set to the root of the USM-T project.

### Available Challenge Types

The following challenge types are available:

*   **And**: Generates a challenge with a formula like `G(..&&.. |-> ..&&..)`
    *   Script: `generate_And.sh`
    *   Example with `nant:2`, `ncon:1`: `G({a_0 & b_0} |-> {c_0})`
*   **Eventually**: Generates a challenge with a formula like `G(..&&.. |-> F ..&&..)`
    *   Script: `generate_Eventually.sh`
    *   Example with `nant:1`, `ncon:2`: `G({a_0} |-> F {b_0 & c_0})`
*   **Next**: Generates a challenge with a formula like `G(..##1.. |-> ..##1..)`
    *   Script: `generate_Next.sh`
    *   Example with `nant:3`, `ncon:3`: `G({a_0 ##1 b_0 ##1 c_0} |-> {d_0 ##1 e_0 ##1 f_0})`
*   **NextAnd**: Generates a challenge with a formula like `G(..#1&.. |-> ..#1&..)`
    *   Script: `generate_NextAnd.sh`
    *   Example with `nant:3`, `ncon:2`: `G({a_0 ##1 b_0 & c_0} |-> {d_0 & e_0})`
*   **Until**: Generates a challenge with a formula like `G(..&&.. |-> ..&&.. U ..&&..)`
    *   Script: `generate_Until.sh`
    *   Example with `nant:3`, `ncon:2`: `G({a_0 & b_0 & c_0} |-> (d_0 & e_0) U (f_0 & g_0))`

To generate the "And" challenge, for example, you would run:

```bash
bash challengeGenerators/generate_And.sh
```

To generate and install it:

```bash
bash challengeGenerators/generate_And.sh 1
```

## Challenge Configuration

The generation scripts (`generate_*.sh`) internally call a wrapper script that takes a configuration string. This string defines the properties of the generated challenge.

Here is an example of a configuration string:

```
{formula : G(..&&.. |-> ..&&..), nant : 2, ncon : 1, nspec: 10, overlap : 0}
```

### Configuration Parameters

*   `formula`: The LTL formula template for the generated specifications. The `..` are placeholders for variables. The implication can be of two types:
    *   `|->`: Simple implication.
    *   `|=>`: Implication where the consequent is delayed by one clock cycle.
*   `nant`: The number of variables in the antecedent part of the formula for each '..' placeholder.
*   `ncon`: The number of variables in the consequent part of the formula for each '..' placeholder.
*   `nspec`: The number of specifications (properties) to generate for the challenge.
*   `overlap`: An integer specifying how many of the generated specifications will share variables with other specifications. This value must be less than `nspec`.