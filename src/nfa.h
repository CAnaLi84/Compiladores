#ifndef NFA_H
#define NFA_H

#include "regex.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/** 
 * @struct State    
 * @brief Represents a state in the NFA. Contains an ID, a flag indicating if it's a final state, 
 * and a list of transitions.
 */
typedef struct state State;

/** 
 * @struct Transition
 * @brief Represents a transition in the NFA. Contains the symbol for the transition, 
 * a pointer to the destination state, and a pointer to the next transition in the list.
 */
typedef struct transition Transition;

/**
 * @struct Alphabet
 * @brief Represents a symbol in the alphabet of the NFA. Contains the symbol and a
 */
typedef struct alphabet {
    char symbol;
    struct alphabet* next;
} Alphabet;

/**
 * @struct NFA
 * @brief Represents a Non-deterministic Finite Automaton (NFA). Contains a pointer to the start state,
 * a list of all states, and a list of symbols in the alphabet.
 */
typedef struct NFA {
    State* start_state;
    State** states;
    size_t state_count;
    Alphabet* alphabet;
} NFA;

#endif /* NFA_H */
