
# The InKS programming model compiler

## Introduction
<div style='text-align: justify'>

**InKS** is a programming model aiming to separate algorithm (what you want to do) and optimizations (how you do it) in HPC codes. It comes with the InKS<sub>PIA</sub> language for the expression of the simulation algorithm and the InKS<sub>PSO</sub> for the description of the optimizations. Multiple InKS<sub>PSO</sub> may implement different optimization strategies for a single InKS<sub>PIA</sub> algorithm.

</div>

This repository provides the InKS source-to-source compiler for InKS files as well as some basic examples. 

## Installation

The InKS compiler have the following dependencies:
* A C++ 11 compiler
* The Integer Set Library - http://isl.gforge.inria.fr/
* The GNU Multiple Precision Arithmetic Library - https://gmplib.org/
* C++ Boost Program options library  1.52 or newer
* The Flex lexical analyser generator
* The Bison parser generator
* Cmake 3.5.1 or newer

The last four are easily installed on most common Linux distribution.

Minimal compilation instructions:
	
	mkdir build; cd build
	cmake ..
	make
	make test

## Usage 

Once compiled, you can then use the InKS compiler (inksC) located in the src directory.
It reads an InKS<sub>PIA</sub> source file and can generate either:
* a generic (automatic) set of optimization choices (*-a* option);
* a specialized set of optimization choices using an InKS<sub>PSO</sub> specification (*-e* option).

It generates an *.hpp* file containing a C++ function which the prototype corresponds to the one specified in the InKS<sub>PIA</sub> algorithm that you may call from your own *.cpp* files.
Note that the generated *.hpp* files use *intel* directives to enforce vectorization or inlining. Hence, using another compiler (such as g++) may greatly impede the performance.

For more help, use *inksC -h*.

## Examples

Examples of the InKS<sub>PIA</sub> algorithm language syntax are available in the example directory (\*.ikf).
Examples of the InKS<sub>PSO</sub> optimization language syntax are available in the example directory (example_\*.cpp).

