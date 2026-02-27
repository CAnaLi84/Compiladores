#include regex.h
#include nfa.h
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
Regex* parse_regex(const char* regex_str);

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
            if(type == TOKEN_INVALID){ //invalid character
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