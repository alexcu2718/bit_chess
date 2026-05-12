# Welcome to bit_chess_cpp

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation Status](https://readthedocs.org/projects/None/badge/)](https://None.readthedocs.io/)

# Prerequisites

Building bit_chess_cpp requires the following software installed:

* A C++23-compliant compiler
* CMake `>= 3.23`
* Doxygen (optional, documentation building is skipped if missing)
* The testing framework [Catch2](https://github.com/catchorg/Catch2) for building the test suite

# Building bit_chess_cpp

The following sequence of commands builds bit_chess_cpp.
It assumes that your current working directory is the top-level directory
of the freshly cloned repository:

```
cmake -B build
cmake --build build
```

The build process can be customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF}` to the `cmake` call:

* `bit_chess_cpp_BUILD_TESTING`: Enable building of the test suite (default: `ON`)
* `bit_chess_cpp_BUILD_DOCS`: Enable building the documentation (default: `ON`)

# Testing bit_chess_cpp

When built according to the above explanation (with `-Dbit_chess_cpp_BUILD_TESTING=ON`),
the C++ test suite of `bit_chess_cpp` can be run using
`ctest` from the build directory:

```
cd build
ctest
```

# Documentation

bit_chess_cpp provides a Sphinx-based documentation, that can
be browsed [online at readthedocs.org](https://bit_chess_cpp.readthedocs.io).
To build it locally, first ensure the requirements are installed by running this command from the top-level source directory:

```
pip install -r doc/requirements.txt
```

Then build the sphinx documentation from the top-level directory:

```
cmake --build build --target sphinx-doc
```

The web documentation can then be browsed by opening `build/doc/sphinx/index.html` in your browser.

## Acknowledgments

This repository was set up using the [SSC Cookiecutter for C++ Packages](https://github.com/ssciwr/cookiecutter-cpp-project).

# bit_chess

Random experiments done out of boredom

Essentially this is just an idea I wrote on a napkin during a corporate meeting.

The main idea is to represent a chess board as 2 u128's, the lower 32 of the first u128  represent the EG black start row, the next 32 bits would be all black pawns. Then Empty+Empty.

The idea is to see if it's feasible to do this, how much trouble have I caused myself?

If it's feasible, granted also, I have not read any chess engine code, nor do I want to, this is a (purposefully!) naive attempt.

I might just write this in C++ instead, would be a curious test, especially regarding constexpr magic.

Anyway, I am slightly drunk from said corporate meeting.

Final/tentative goal, abstract the  2*u128's's as a single SIMD avx2 vector ( aka __m256i in C),

**(x86 specific unfortunately, I don't own ARM and this is a dumb test)*

The end result, would excitingly be a able to model state transitions of the board via simple bit operations on a 256bit integer. This would allow a potential massive speedup in certain areas, because stack use would be minimised!

***I PROBABLY WON'T LOOK AT THIS AGAIN TILL I GET BORED**
