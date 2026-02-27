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


/* This function receive a string regex, return its tokens and the size*/
Token* tokenize(const char* input, size_t* out_count){
    if(input != NULL && out_count != NULL){
        size_t len = strlen(input);
        Token* tokens = malloc(len * sizeof(Token));
        if(tokens == NULL){
            return NULL;
        }
        size_t count = 0;
        for(size_t i = 0; i < len; i++){
            char c = input[i];
            if(c == ' ' || c == '\t' || c == '\n'){
                continue; // Skip whitespace
            }
            tokens[count].value = c;
            tokens[count].type = get_token_type(c);
            count++;
        }
        *out_count = count;
        return tokens;
    }
    if (out_count)
    {
        *out_count = 0;
    }
    return NULL;
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

