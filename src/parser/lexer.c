#include "../include/minishell.h"

/*
 * simple lexer (lexical analyzer) using a linked list data structure to represent tokens. 
 */
t_token *lexer(char *line)
{
    t_token *tokens;    // Linked list to store tokens
    int i;               // Index for iterating through the input line
    enum e_token_state state;  // Current state of the lexer (enum e_state)

    i = 0;
    state = DEFAULT;     // Initialize lexer state to GENERAL
    tokens = NULL;       // Initialize tokens to NULL
    tokens = init_list(tokens);  // Initialize the linked list

    if (tokens)
    {
        while (line[i])  // Iterate through the characters of the input line
            i = tokenize(tokens, line, i, &state);  // Process characters and create tokens
    }

    return (tokens);  // Return the linked list of tokens
}