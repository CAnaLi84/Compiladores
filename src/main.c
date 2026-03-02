#include <stdio.h>
#include <stdlib.h>
#include "regex.h"

int main() {
    const char* regex = "a(b|c)*d";
    NFA* nfa = parse_regex(regex);
    if (nfa == NULL) {
        fprintf(stderr, "Failed to parse regex: %s\n", regex);
        return EXIT_FAILURE;
    }
    printf("NFA successfully created for regex: %s\n", regex);
    free_nfa(nfa);
    return EXIT_SUCCESS;
}