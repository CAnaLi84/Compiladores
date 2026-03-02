# Regular Expression to NFA

## Ana Lilia Carballido Camacateco

This project aims to implement a compiler with all its stages using the **C** programming language.
The developed compiler processes a language created for academic purposes, going through the following stages:

- Lexical analysis
- Semantic analysis
- Syntax analysis

---
For this project, the **GitHub Flow** branching strategy will be used.

---

This project implements a **Regular Expression to NFA (Non-deterministic Finite Automaton)** converter using **Thompson's Construction Algorithm**.

Given a regular expression as input, the program:

1. Tokenizes the input
2. Converts it to postfix notation (if applicable)
3. Builds the corresponding NFA
4. Outputs the constructed automaton

The project is written in C and built using CMake.

---

## Alphabet Definition

The supported alphabet consists of:

- Lowercase letters: `a–z`
- Uppercase letters: `A–Z`
- Digits: `0–9`

Each character represents a valid symbol in the regular expression.

---

## Supported Operations

The following regular expression operators are supported:

| Operator | Description |
|----------|------------|
| `|`      | Union (alternation) |
| `.`      | Explicit concatenation |
| `*`      | Kleene star (zero or more) |
| `(` `)`  | Grouping |

Concatenation may be handled explicitly or implicitly depending on implementation.

---

## Main Files
### `regex.h`
- Token definitions
- Regex-related structures
- Function declarations for tokenization and parsing

### `regex.c`
- Implementation of the tokenizer
- Conversion to postfix notation (if applicable)

### `nfa.h`
- Definition of `State` and `NFA` structures
- Function declarations for building and freeing NFAs

### `nfa.c`
- Implementation of Thompson's Construction
- State creation
- Transition management
- Memory cleanup functions

---
## Requirements

To build this project, you need:

- **CMake >= 3.16**
- A C compiler (GCC, Clang, etc.)
- `make`

## Build Instructions

From the root directory of the project:

mkdir build
cd build
cmake ..
make


This will generate the executable inside the `build/` directory.

---


The program will construct and display the corresponding NFA.

---

## What the Project Does

This project demonstrates:

- Tokenization
- Operator precedence handling
- Stack-based parsing with Shunting Yard algorithm
- Thompson’s algorithm for NFA construction

The resulting NFA contains:

- A start state
- An accept state
- Transitions including epsilon transitions
- Dynamically linked states



