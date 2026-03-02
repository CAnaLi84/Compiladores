#include "nfa.h"
#include <stdlib.h>

/*Program to model the conversion of a regular expression written in an 
*implicit concatenation infix form to a non-deterministic finite automaton.
*/
static int global_state_id = 0;

/**
 * @brief Creates a new state with a unique ID and specified final state status.
 * @param is_final A boolean indicating whether the state is a final state.
 * @return State* A pointer to the newly created state, or NULL if memory allocation fails
 */
State* new_state(bool is_final) {
    State* state = malloc(sizeof(State));
    if (state == NULL) {
        return NULL;
    }
    state->id = global_state_id++; //Assign a unique ID to the state
    state->is_final = is_final; //Set the final state status
    state->transitions = NULL; //Initialize the transitions list to NULL
    state->next = NULL; //Initialize the next pointer to NULL
    return state;
}

/********************Functions for NFA manipulation********************/
/** * @brief Adds a transition from one state to another with a given symbol.
 * @param from A pointer to the source state.
 * @param to A pointer to the destination state.
 * @param symbol The symbol for the transition. Use EPSILON_SYMBOL for epsilon transitions.
 */
void add_transition(State* from, State* to, char symbol) {
    Transition* transition = malloc(sizeof(Transition));
    if (transition == NULL) {   
        return;
    }
    transition->symbol = symbol; //Set the transition symbol
    transition->to = to;         //Set the destination state
    transition->next = from->transitions; //Insert the new transition at the beginning of the transitions list
    from->transitions = transition;       //Update the head of the transitions list to the new transition
}

/**
 * @brief Frees the memory allocated for a transition.
 * @param transition A pointer to the transition to be freed.
 */
void free_transition(Transition* transition) {
    if (transition != NULL) {
        free(transition);
    }
}

/**
 * @brief Frees the memory allocated for a list of transitions.
 * @param transition A pointer to the head of the transitions list to be freed.
 */
static void free_transitions(Transition* transition) {
    while (transition != NULL) {
        Transition* temp = transition;
        transition = transition->next;
        free_transition(temp);
    }
}

/**
 * @brief Frees the memory allocated for a list of states.
 * @param s A pointer to the head of the states list to be freed.
 */
static void free_state_list(State* s) {
    while (s) {
        free_transitions(s->transitions);
        State* tmp = s;
        s = s->next;
        free(tmp);
    }
}

/** * @brief Frees the memory allocated for an NFA, including all states and transitions.
 * @param nfa A pointer to the NFA to be freed.
 */
void free_nfa(NFA* nfa) {
    if (nfa == NULL) {
        return;
    }
    State* current_state = nfa->states;
    while (current_state != NULL) {
        Transition* current_transition = current_state->transitions;
        while (current_transition != NULL) {
            Transition* temp_transition = current_transition;
            current_transition = current_transition->next;
            free(temp_transition); // Free each transition
        }
        State* temp_state = current_state;
        current_state = current_state->next;
        free(temp_state); // Free each state
    }
    free(nfa); // Finally, free the NFA structure itself
}

/************Construction of NFA structures*************/
/**
 * @brief Constructs an NFA that recognizes a single symbol.
 * @param symbol The symbol that the NFA should recognize. Use EPSILON_SYMBOL for an epsilon transition.
 * @return NFA* A pointer to the constructed NFA, or NULL if memory allocation fails.
 */
NFA* nfa_symbol(char symbol) {
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    State* s0 = new_state(false);
    State* accept = new_state(true);
    if (s0 == NULL || accept == NULL) { //Check for state creation failure
        free(nfa);
        return NULL;
    }
    add_transition(s0, accept, symbol); //Add transition from start to accept with the given symbol
    nfa->start = s0; //Set the start state of the NFA
    nfa->states = s0; //Initialize the states list with the start state
    s0->next = accept; //Add the accept state to the states list
    return nfa;
}


/**
 * @brief Constructs an NFA that recognizes the union of two NFAs.
 * @param nfa1 A pointer to the first NFA.
 * @param nfa2 A pointer to the second NFA.
 * @return NFA* A pointer to the constructed NFA, or NULL if memory allocation fails.
 */
NFA* nfa_union(NFA* nfa1, NFA* nfa2) {
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    State* start = new_state(false);
    if (start == NULL) { //Check for state creation failure
        free(nfa);
        return NULL;
    }
    add_transition(start, nfa1->start, EPSILON_SYMBOL); //Add epsilon transition from new start to nfa1 start
    add_transition(start, nfa2->start, EPSILON_SYMBOL); //Add epsilon transition from new start to nfa2 start
    nfa->start = start; //Set the new start state of the NFA
    nfa->states = start; //Initialize the states list with the new start state
    //Append states of nfa1 and nfa2 to the states list of the new NFA
    State* current_state = start;
    while (current_state->next != NULL) {
        current_state = current_state->next;
    }
    current_state->next = nfa1->states; //Append states of nfa1
    current_state = current_state->next;
    while (current_state->next != NULL) {
        current_state = current_state->next;
    }
    current_state->next = nfa2->states; //Append states of nfa2
    return nfa;
}


/**
 * @brief Constructs an NFA that recognizes the concatenation of two NFAs.
 * @param nfa1 A pointer to the first NFA.
 * @param nfa2 A pointer to the second NFA.
 * @return NFA* A pointer to the constructed NFA, or NULL if memory allocation fails.
 */
NFA* nfa_concatenate(NFA* nfa1, NFA* nfa2) {
    //Add epsilon transitions from all final states of nfa1 to the start state of nfa2
    State* current_state = nfa1->states;
    while (current_state != NULL) {
        if (current_state->is_final) {
            add_transition(current_state, nfa2->start, EPSILON_SYMBOL);
            current_state->is_final = false; //The final states of nfa1 are no longer final after concatenation
        }
        current_state = current_state->next;
    }
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    nfa->start = nfa1->start; //The start state of the new NFA is the start state of nfa1
    nfa->states = nfa1->states; //Initialize the states list with the states of nfa1
    //Append states of nfa2 to the states list of the new NFA
    current_state = nfa->states;
    while (current_state->next != NULL) {
        current_state = current_state->next;
    }
    current_state->next = nfa2->states; //Append states of nfa2
    return nfa;
}

/**
 * @brief Constructs an NFA that recognizes the Kleene star of an NFA.
 * @param nfa1 A pointer to the NFA to be starred.
 * @return NFA* A pointer to the constructed NFA, or NULL if memory allocation fails.
 */
NFA* nfa_star(NFA* nfa1) {
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    State* start = new_state(false);
    if (start == NULL) { //Check for state creation failure
        free(nfa);
        return NULL;
    }
    add_transition(start, nfa1->start, EPSILON_SYMBOL); //Add epsilon transition from new start to nfa1 start
    //Add epsilon transitions from all final states of nfa1 back to its start state and to the new start state
    State* current_state = nfa1->states;
    while (current_state != NULL) {
        if (current_state->is_final) {
            add_transition(current_state, nfa1->start, EPSILON_SYMBOL); //Loop back to start of nfa1
            add_transition(current_state, start, EPSILON_SYMBOL); //Allow transition to new start for zero occurrences
            current_state->is_final = false; //The final states of nfa1 are no longer final after applying star
        }
        current_state = current_state->next;
    }
    nfa->start = start; //Set the new start state of the NFA
    nfa->states = start; //Initialize the states list with the new start state
    //Append states of nfa1 to the states list of the new NFA
    current_state = start;
    while (current_state->next != NULL) {
        current_state = current_state->next;
    }
    current_state->next = nfa1->states; //Append states of nfa1
    return nfa;
}

/**
 * @brief Constructs an NFA that recognizes the one-or-more repetition of an NFA.
 * @param nfa1 A pointer to the NFA to be repeated.
 * @return NFA* A pointer to the constructed NFA, or NULL if memory allocation fails.
 */
NFA* nfa_plus(NFA* nfa1) {
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    State* start = new_state(false);
    if (start == NULL) { //Check for state creation failure
        free(nfa);
        return NULL;
    }
    add_transition(start, nfa1->start, EPSILON_SYMBOL); //Add epsilon transition from new start to nfa1 start
    //Add epsilon transitions from all final states of nfa1 back to its start state
    State* current_state = nfa1->states;
    while (current_state != NULL) {
        if (current_state->is_final) {
            add_transition(current_state, nfa1->start, EPSILON_SYMBOL); //Loop back to start of nfa1
            current_state->is_final = false; //The final states of nfa1 are no longer final after applying plus
        }
        current_state = current_state->next;
    }
    nfa->start = start; //Set the new start state of the NFA
    nfa->states = start; //Initialize the states list with the new start state
    //Append states of nfa1 to the states list of the new NFA
    current_state = start;
    while (current_state->next != NULL) {
        current_state = current_state->next;
    }
    current_state->next = nfa1->states; //Append states of nfa1
    return nfa;
}

/**
 * @brief Constructs an NFA that recognizes the zero-or-one repetition of an NFA.
 * @param nfa1 A pointer to the NFA to be repeated.
 * @return NFA* A pointer to the constructed NFA, or NULL if memory allocation fails.
 */
NFA* nfa_question(NFA* nfa1) {
    NFA* nfa = malloc(sizeof(NFA));
    if (nfa == NULL) {
        return NULL;
    }
    State* start = new_state(false);
    if (start == NULL) { //Check for state creation failure
        free(nfa);
        return NULL;
    }
    add_transition(start, nfa1->start, EPSILON_SYMBOL); //Add epsilon transition from new start to nfa1 start
    add_transition(start, start, EPSILON_SYMBOL); //Allow transition to new start for zero occurrences
    nfa->start = start; //Set the new start state of the NFA
    nfa->states = start; //Initialize the states list with the new start state
    //Append states of nfa1 to the states list of the new NFA
    State* current_state = start;
    while (current_state->next != NULL) {
        current_state = current_state->next;
    }
    current_state->next = nfa1->states; //Append states of nfa1
    return nfa;
}

/**
 * @brief Constructs an NFA that recognizes the empty string (epsilon).
 * @return NFA* A pointer to the constructed NFA.
 */
NFA* nfa_epsilon() {
    return nfa_symbol(EPSILON_SYMBOL);   
}





