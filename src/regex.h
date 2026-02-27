#ifndef REGEX_H
#define REGEX_H
#include "nfa.h"
#include <stdbool.h>

/* Operator symbols*/

#define OP_UNION '|'
#define OP_CONCAT '.'
#define OP_STAR '*'
#define OP_PLUS '+'
#define OP_QUESTION '?'

#define EPSILON_SYMBOL '\0'

#define LEFT_PAREN '('
#define RIGHT_PAREN ')'


/**
 * @enum TokenType
 * @brief Represents the different types of tokens in a regular expression.
 */
typedef enum {
    TOKEN_SYMBOL,
    TOKEN_UNION,
    TOKEN_CONCAT,
    TOKEN_STAR,
    TOKEN_PLUS,
    TOKEN_QUESTION,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EPSILON,
    TOKEN_INVALID,
} TokenType;


/**
 * @struct Token
 * @brief Represents a token extracted from a regular expression.
 */
typedef struct {
    TokenType type;
    char value;  //for symbols
} Token;

/**
 * @struct Regex
 * @brief Opaque structure representing a parsed regular expression.
 */
typedef struct Regex Regex;

/**
 * @brief Determines the token type corresponding to a character.
 * @param c The input character.
 * @return TokenType The type associated with the character.
 */
TokenType get_token_type(char c);

/**
 * @brief Returns the precedence level of a given token type.
 * -Higher values indicate higher precedence.
 * @param t The token type.
 * @return int The precedence value.
 */
int precedence(TokenType t);

/* ---------Funtions for parsing pipeline*/

/**
 * @brief Validates the basic syntax of a regular expression string.
 * This function checks for structural correctness such as balanced
 * parentheses and valid operator placement.
 * @param regex The input regular expression string.
 * @return true If the regex is syntactically valid.
 * @return false Otherwise.
 */
bool validate_regex(const char* regex);

/**
 * @brief Converts implicit concatenation into explicit concatenation.
 * Memory for the returned string is dynamically allocated and must
 * be freed by the caller.
 * @param regex The input infix regular expression.
 * @return char* A new string with explicit concatenation operators.
 */
char* implicit_to_explicit(const char* regex);

/**
 * @brief Converts an infix regular expression into postfix notation
 * using the Shunting Yard algorithm.
 * The input must already contain explicit concatenation operators.
 * Memory for the returned string is dynamically allocated and must
 * be freed by the caller.
 * @param input The explicit infix expression.
 * @return char* The postfix representation.
 */
char* shunting_yard(const char* input);

/**
 * @brief Parses a regular expression string and builds the corresponding NFA.
 * 
 * @param regex_str The input regular expression string.
 * @return NFA* Pointer to the constructed NFA, or NULL on error.
 */
NFA* parse_regex(const char* regex_str);

#endif