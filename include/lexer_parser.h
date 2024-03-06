#ifndef PARSER_H
# define PARSER_H

#include <stdlib.h>

// enum e_token_type
// {
//     WORD = -1,        // Represents a word token
//     WHITESPACE = ' ', // Represents a white space token
//     QOUTE = '\'',      // Represents a single quote token
//     DQUOTE = '\"', // Represents a double quote token
//     ENV = '$',         // Represents an environment variable token
//     PIPE_LINE = '|',   // Represents a pipe line token
//     REDIR_IN = '<',    // Represents a input redirection token
//     REDIR_OUT = '>',   // Represents an output redirection token
//     HERE_DOC,         // Represents a here document token, often used for input redirection.
//     DREDIR_OUT,       // epresents a double output redirection token (>>), used to append output to a file.
//     SEMICOLON = ';',   // Represents a semicolon token
//     AND = '&',         // Represents an and token
//     OR = '|',          // Represents an or token
//     NEWLINE = '\n',    // Represents a newline token
//     EQUALS = '=',          // Represents an equals sign token
//     UNDERSCORE = '_',  // Represents an underscore token
// };



#define TOKEN_AND "&"
#define TOKEN_DOR "||"
#define TOKEN_DQUOTE "\""
#define TOKEN_DREDIR_OUT ">>"
#define TOKEN_HEREDOC "<<"
#define TOKEN_ENV "$"
#define TOKEN_EQUALS "="
#define TOKEN_NEWLINE "\n"
#define TOKEN_OR "|"
#define TOKEN_PIPE "|"
#define TOKEN_QOUTE "\'"
#define TOKEN_REDIR_IN "<"
#define TOKEN_REDIR_OUT ">"
#define TOKEN_SEMICOLON ";"
#define TOKEN_TAB "\t"
#define TOKEN_UNDERSCORE "_"
#define TOKEN_WHITESPACE " "
#define TOKEN_WORD "WORD"


enum e_token_state
{
	DEFAULT,
	IN_QUOTES,
	IN_DQUOTES
};

typedef struct s_token
{
    char *type;             // Represents the type of the token
    char *contents;          // Represents the content or value of the token
    int len;                // Represents the length of the content
    enum e_token_state state;     // Represents the state of the lexer when processing this token
    struct s_token *prev;    // Pointer to the previous token in the linked list
    struct s_token *next;
} t_token;

t_token	*new_token(char* type, char *contents, int len, enum e_token_state state);
void	token_add_back(t_token **token_lst, t_token *new);
t_token	*token_find(t_token *token_lst, char *type);
void	token_free(t_token **token_lst);

int     tokenize(t_token *tokens, char *str, int i, enum e_token_state *state);
char	*get_redir_symbol(enum e_token_type type);
int	    is_redir(enum e_token_type type);

#endif