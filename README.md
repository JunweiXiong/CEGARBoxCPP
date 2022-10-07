# CEGARBoxCPP

## Authorship
All files in this repository were authored by Junwei Xiong.
## Dependencies
This project requires minisat installed to run and compile. Instruction for installing minisat are available [here](https://github.com/niklasso/minisat).

## Compile
A statically compiled file is already available in the repo - compiled on Ubuntu 20.04.3.

Run ``./compile.sh`` to compile CEGARBox

## Input Formula
CEGARBox accepts file input. Input is terminated by a newline and valid input formula are defined by the following grammar:
```
Index ::= Nat || -Nat
Atom ::= Alphanumeric String
Formula ::=
 Atom || $true || $false || ~Formula ||
 [Index] Formula || <Index> Formula || []Formula || <>Formula || 
 Formula | Formula || Formula & Formula || Formula => Formula || 
 Formula <=> Formula || (Formula) 
```

Here, negative numbers are used to define converse for tense logic. For example ``[-1]`` is the converse of ``[1]``.

## Run Theorem Prover

``./main -f <input_file> [options]``

Options:

* Reflexivity: ``--reflexive`` or ``-t``
* Symmetry: ``--symmetric`` or ``-b``
* Transitivity: ``--transitive`` or ``-4``
* Seriality: ``--serial`` or ``-d``
* Euclidean: ``--euclidean`` or ``-e``
* Valid (whether input formula is valid): ``--valid`` or ``-a``
* Tense: ``--tense`` or `-n`
* Verbose: ``--verbose`` or `-v`

## Benchmarks

MQBF, 3CNF and LWB_K benchmarks can be downloaded from [here](http://www.cril.univ-artois.fr/~montmirail/mosaic/#)
ALC benchmarks can be downloaded from [here](https://web.archive.org/web/20190305011522/http://users.cecs.anu.edu.au/~rpg/BDDTab/)

Use ``Experiment/convert.py`` to convert the benchmarks into a valid format.

## Experiments

Experiments can be run using Experiment/experiment.py and ``Exeriment/experiment_reflexive.py``
