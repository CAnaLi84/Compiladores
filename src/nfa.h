#ifndef NFA_H
#define NFA_H

#include <stdbool.h>


/**
 * @brief Epsilon transition symbol
 */
#define EPSILON_SYMBOL '#'

typedef struct State State;
typedef struct Transition Transition;

/** 
 * @struct Transition
 * @brief Represents a transition in the NFA. Contains the symbol for the transition, 
 * a pointer to the destination state, and a pointer to the next transition in the list.
 */
typedef struct Transition{
    char symbol;              //Símbolo de transición. Epsilon es el caracter '\0'
    State* to;                //Estado de llegada
    Transition* next;         //Lista de transiciones desde el mismo estado
}Transition;

/** 
 * @struct State    
 * @brief Represents a state in the NFA. Contains an ID, a flag indicating if it's a final state, 
 * a list of transitions from this state, and a pointer to the next state in the list of states.
 */
typedef struct State{
    int id;
    bool is_final;
    Transition* transitions;
    struct State* next; // Pointer to the next state in the list of states
}State;

/**
 * @struct NFA
 * @brief Represents a Non-deterministic Finite Automaton.
 */
typedef struct {
    State* start;        /*Initial state */
    State* states;       /*Linked list of all states */
} NFA;

State* new_state(bool is_final);
void add_transition(State* from, State* to, char symbol);
void free_nfa(NFA* nfa);

#endif /* NFA_H */
