#ifndef REGEX_H
#define REGEX_H
#include <stdbool.h>

//Definition of regex and operators 


//Operators:
/* Union symbol Ex: a|b*/
#define op_union '|'
/* Concatenation symbol Ex: a.b*/
#define op_concatenation '.'
/* Star symbol eX: a* */
#define op_star '*'
/* Plus symbol Ex: b+*/
#define op_plus '+'

/* Definition of epsilon symbol \epsilon */
#define epsilon_symbol '\0'

/*Left parenthesis symbol*/
#define left_parenthesis '('

/*Right parenthesis symbol*/
#define right_parenthesis ')'


typedef enum {
    TOKEN_SYMBOL,
    TOKEN_UNION,
    TOKEN_CONCAT,
    TOKEN_STAR,
    TOKEN_PLUS,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EPSILON,
    TOKEN_INVALID
} TokenType;


typedef struct {
    TokenType type;
    char value;   //for symbols
} Token;


TokenType get_token_type(char c) {

    if (c >= 'a' && c <= 'z')
        return TOKEN_SYMBOL;

    if (c >= '0' && c <= '9')
        return TOKEN_SYMBOL;

    if (c == '|')
        return TOKEN_UNION;

    if (c == '.')
        return TOKEN_CONCAT;

    if (c == '*')
        return TOKEN_STAR;

    if (c == '+')
        return TOKEN_PLUS;

    if (c == '(')
        return TOKEN_LPAREN;

    if (c == ')')
        return TOKEN_RPAREN;

    if (c == '\0')   
        return TOKEN_EPSILON;

    return TOKEN_INVALID;
}

/*Define precedence*/
int precedence(TokenType t) {
    if (t == TOKEN_STAR || t == TOKEN_PLUS) return 3; 
    if (t == TOKEN_CONCAT) return 2;
    if (t == TOKEN_UNION) return 1; 
    return 0;
}


bool validate_regex(const char* regex);   
void insert_concatenation(const char* input, char* output);
void shunting_yard(const char* input, char* output);

#endif