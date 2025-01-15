
The official repo of the Universal Specification Miner Tester (USM-T) tool.

  

## Table of contents

[Project info](#project-info)

[Quick start](#quick-start)
1. [Install dependencies](#dependencies)
2. [Build the project](#build-the-project)
3. [Run default tests](#run-default-tests)

[How to use the tool](#ow-to-use-the-tool)

[The configuration file](#the-configuration-file)

[Optional arguments](#optional-arguments)

[Citations](#citations)

## Project info

This project introduces a robust framework designed for evaluating and comparing LTL (Linear Temporal Logic) specification miners. Traditional approaches often struggle with subjective assessments and intricate configurations; our solution addresses these issues by offering a structured methodology to assess the quality of specifications through both semantic and syntactic analyses.

# Quick start

For now, we support only Linux with gcc and clang (c++17) and cmake 3.14+.

## Dependencies
* [spotLTL](https://spot.lrde.epita.fr/install.html)
* [antlr4-runtime](https://www.antlr.org)
* [gedlib](https://dbblumenthal.github.io/gedlib/)
* [z3](https://github.com/Z3Prover/z3.git)

  

### Ubuntu

```
sudo apt-get install -y uuid-dev pkg-config
```

### Third party

* Install all dependencies. All these dependencies will be compiled from source.
* This will not dirty your system, as all the dependencies will be installed in the third\_party directory.

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

Run cmake.

Build without SpotLTL: the generate binaries will comply with the MIT license.
Evaluation of temporal formulas will follow a modular-based approach.
```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Build
```
make
```


# How to use the tool

Set the environment variables for the tool

```
bash scripts/setupEnvironment.sh 
```

Download the miners

```
bash scripts/downloadMiners.sh
```

# The configuration file
 
# Optional arguments



# Citations
If you need to reference HARM in an academic publication, refer to the following paper:
```
```
