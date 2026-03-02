#include "regex.h"
#include "nfa.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Auxilia functions*/
Token* tokenize(const char* input, size_t* out_count);
TokenType get_token_type(char c);
int precedence(TokenType t);

/* Parsing helpers functions*/
bool validate_regex(const char* regex);
char* implicit_to_explicit(const char* regex);
char* shunting_yard(const char* input);

/* Thmpson algorithm*/
static NFA* build_nfa_from_postfix(const char* postfix);

/* Final parser*/
NFA* parse_regex(const char* regex_str);

/*---------------------IMPLEMENTATION------------------*/

/*This function changes a regex to a NFA doing explicit concatenation, postfix and build the nfa*/
NFA* parse_regex(const char* regex_str) {
    if (!validate_regex(regex_str))
        return NULL;
    char* explicit = implicit_to_explicit(regex_str);
    if (!explicit)
        return NULL;
    char* postfix = shunting_yard(explicit);
    free(explicit);
    if (!postfix)
        return NULL;
    NFA* nfa = build_nfa_from_postfix(postfix);
    free(postfix);
    return nfa;
}

/* This function tokenizes the input string into an array of tokens. 
It also counts the number of tokens and returns it through the out_count parameter.
*/
Token* tokenize(const char* input, size_t* out_count){
    if(input != NULL && out_count != NULL){ 
        size_t len = strlen(input); 
        Token* tokens = malloc(len * sizeof(Token)); // Allocate memory for tokens
        if(tokens == NULL){ //check for allocation failure
            return NULL;
        }
        size_t count = 0;
        for(size_t i = 0; i < len; i++){
            char c = input[i];
            if(c == ' '){ //skip spaces
                continue; 
            }
            if(get_token_type(i) == TOKEN_INVALID){ //invalid character
                free(tokens);
                return NULL;
            }
            tokens[count].value = c;//set the token value
            tokens[count].type = get_token_type(c);//and the token type
            count++;
        }
        if (count == 0) { //no valid tokens found
        free(tokens);
        return NULL;
        }
        *out_count = count;
        return tokens;
    } else {//invalid input
        return NULL;
    }
}

/* This function return the type of a char*/
TokenType get_token_type(char c){
    if(c == OP_CONCAT){
        return TOKEN_CONCAT;
    } else if(c == OP_STAR){
        return TOKEN_STAR;
    } else if(c == OP_PLUS){
        return TOKEN_PLUS;
    } else if(c == OP_QUESTION){    
        return TOKEN_QUESTION;
    } else if(c == OP_UNION){
        return TOKEN_UNION;
    } else if(c == LEFT_PAREN){
        return TOKEN_LPAREN;
    } else if(c == RIGHT_PAREN){
        return TOKEN_RPAREN;
    } else if(c == EPSILON_SYMBOL){
        return TOKEN_EPSILON;
    } else {
        return TOKEN_SYMBOL; //Any other character is considered a symbol
    }
}

/* This function returns the precedence of a token type. The higher the number, the higher the precedence*/
int precedence(TokenType t) {
    switch(t) {
        case TOKEN_STAR:
        case TOKEN_PLUS:
        case TOKEN_QUESTION:
            return 3;
        case TOKEN_CONCAT:
            return 2;
        case TOKEN_UNION:
            return 1;
        default:
            return 0;
    }
}

/*This function returns true if the regex is valid, false otherwise.
It checks for balanced parentheses and valid operator placement. */
bool validate_regex(const char* regex) {
    if (regex == NULL) { //void regex is not valid
        return false;
    }
    size_t len = strlen(regex);
    if (len == 0) {
        return false;
    }
    int parentheses_count = 0;
    for (size_t i = 0; i < len; i++) {
        char c = regex[i];
        if (c == LEFT_PAREN) {
            parentheses_count++;
        } else if (c == RIGHT_PAREN) {
            parentheses_count--;
            if (parentheses_count < 0) {//There are more right parentheses than left ones
                return false;
            }
        } else if (c == OP_STAR || c == OP_PLUS || c == OP_QUESTION) {
            if (i == 0 || regex[i-1] == LEFT_PAREN || regex[i-1] == OP_UNION || regex[i-1] == OP_CONCAT) {
                return false; // Invalid position for operator, because it cannot be the first character, or follow an opening parenthesis, union or concatenation operator
            }
        } else if (c == OP_UNION && i > 0 && regex[i-1] != RIGHT_PAREN && regex[i-1] != OP_STAR && regex[i-1] != OP_PLUS && regex[i-1] != OP_QUESTION && regex[i-1] != EPSILON_SYMBOL) {
            return false; // Invalid position for union operator, because it cannot follow a symbol, a closing parenthesis, a star, a plus, a question mark or an epsilon symbol
        }
    }
    return parentheses_count == 0; //Like count must be zero for balanced parentheses
}

/*Funtion to convert implicit concatenation into explicit concatenation.
The rules for implicit concatenation are:
1. If a character is not an operator and is not a closing parenthesis, it is implicitly concatenated with the previous character.
2. If a closing parenthesis is followed by a character that is not an operator and not a closing parenthesis, it is implicitly concatenated with the following character.
3. If an opening parenthesis is followed by a character that is not an operator and not a closing parenthesis, it is implicitly concatenated with the following character.
4. If an operator is followed by an opening parenthesis, it is implicitly concatenated with the following character.
5. If an operator is followed by another operator, it is invalid.
*/
char* implicit_to_explicit(const char* regex) {
    if (regex == NULL) {
        return NULL;
    }
    size_t len = strlen(regex);
    char* result = malloc(len * 2 + 1); //Allocate space for explicit concatenation
    if (result == NULL) {
        return NULL;
    }
    size_t out_index = 0;
    for (size_t i = 0; i < len; i++) {
        char c = regex[i];
        if (c == LEFT_PAREN || c == OP_UNION || c == OP_QUESTION || c == OP_PLUS || c == OP_STAR) {
            result[out_index++] = c;
        } else if (c == RIGHT_PAREN && i > 0 && regex[i-1] != LEFT_PAREN && regex[i-1] != OP_UNION && regex[i-1] != OP_QUESTION && regex[i-1] != OP_PLUS && regex[i-1] != OP_STAR) {
            result[out_index++] = OP_CONCAT;
            result[out_index++] = c;
        } else if (i > 0 && regex[i-1] != LEFT_PAREN && regex[i-1] != OP_UNION && regex[i-1] != OP_QUESTION && regex[i-1] != OP_PLUS && regex[i-1] != OP_STAR && c != RIGHT_PAREN) {
            result[out_index++] = OP_CONCAT;
            result[out_index++] = c;
        } else {
            result[out_index++] = c;
        }
    }
    result[out_index] = '\0';
    return result;
}

/*Function to implements Shunting Yard Algorithm for converting infix to postfix notation*/
char* shunting_yard(const char* input) {
    if (input == NULL) {
        return NULL;
    }
    size_t len = strlen(input);
    char* buffer = malloc(len * 2 + 1); //Allocate space for output empty
    if (buffer == NULL) {
        return NULL;
    }
    char* stack = malloc(len + 1); //Stack for operators empty
    if (stack == NULL) {
        free(buffer);
        return NULL;
    }
    size_t out_index = 0, stack_index = 0;
    for (size_t i = 0; i < len; i++) {
        char c = input[i];
        TokenType type = get_token_type(c); //We have to know the type of the token to know how to handle it
        if (type == TOKEN_SYMBOL || type == TOKEN_EPSILON) { //If it's an operand, add it to the output
            buffer[out_index++] = c; //Add symbols directly to output
        } else if (type == TOKEN_LPAREN) {
            stack[stack_index++] = c; //Push left parenthesis to stack
        } else if (type == TOKEN_RPAREN) {
            while (stack_index > 0 && stack[stack_index - 1] != LEFT_PAREN) {
                buffer[out_index++] = stack[--stack_index]; //Pop operators to output until left parenthesis is found
            }
            if (stack_index > 0 && stack[stack_index - 1] == LEFT_PAREN) {
                stack_index--; //Pop the left parenthesis from the stack
            }
        } else { //Handle operator *, +, ?, ., |
            while (stack_index > 0 && precedence(get_token_type(stack[stack_index - 1])) >= precedence(type)) {
                buffer[out_index++] = stack[--stack_index]; // Pop operators with higher or equal precedence to output
            }
            stack[stack_index++] = c; // Push current operator to stack
        }
    }
    while (stack_index > 0) {
        buffer[out_index++] = stack[--stack_index]; // Pop remaining operators to output
    }
    buffer[out_index] = '\0';
    free(stack);
    return buffer;
}

/*Funcion to implement Thompson algorithm*/
static NFA* build_nfa_from_postfix(const char* postfix) {
    if (postfix == NULL) {
        return NULL;
    }
    size_t len = strlen(postfix);
    NFA** stack = malloc(len * sizeof(NFA*)); //Stack for NFAs
    if (stack == NULL) {
        return NULL;
    }
    size_t stack_index = 0;
    for (size_t i = 0; i < len; i++) {
        char c = postfix[i];
        TokenType type = get_token_type(c);
        if (type == TOKEN_SYMBOL || type == TOKEN_EPSILON) {
            NFA* nfa = nfa_symbol(c); //Create an NFA for the symbol
            if (nfa == NULL) { //Check for allocation failure
                free(stack);
                return NULL;
            }
            stack[stack_index++] = nfa; //Push the NFA to the stack
        } else if (type == TOKEN_UNION) {
            if (stack_index < 2) { //Not enough NFAs on the stack for union
                free(stack);
                return NULL;
            }
            NFA* nfa2 = stack[--stack_index]; //Pop the top two NFAs from the stack
            NFA* nfa1 = stack[--stack_index];
            NFA* nfa = nfa_union(nfa1, nfa2); //Create a new NFA for the union of the two NFAs
            if (nfa == NULL) { //Check for allocation failure
                free(stack);
                return NULL;
            }
            stack[stack_index++] = nfa; //Push the new NFA to the stack
        } else if (type == TOKEN_CONCAT) {
            if (stack_index < 2) { //Not enough NFAs on the stack for concatenation
                free(stack);
                return NULL;
            }
            NFA* nfa2 = stack[--stack_index]; //Pop the top two NFAs from the stack
            NFA* nfa1 = stack[--stack_index];
            NFA* nfa = nfa_concatenate(nfa1, nfa2); //Create a new NFA for the concatenation of the two NFAs
            if (nfa == NULL) { //Check for allocation failure
                free(stack);
                return NULL;
            }
            stack[stack_index++] = nfa; //Push the new NFA to the stack
        } else if (type == TOKEN_STAR) {
            if (stack_index < 1) { //Not enough NFAs on the stack for star
                free(stack);
                return NULL;    
            }
            NFA* nfa1 = stack[--stack_index]; //Pop the top NFA from the stack
            NFA* nfa = nfa_star(nfa1); //Create a new NFA for the star of the popped NFA
            if (nfa == NULL) { //Check for allocation failure
                free(stack);
                return NULL;
            }
            stack[stack_index++] = nfa; //Push the new NFA to the stack
        }
    }
    if (stack_index != 1) { //If there is not exactly one NFA on the stack, return NULL
        free(stack);
        return NULL;
    }
    NFA* result = stack[0];
    free(stack);
    return result;
}
