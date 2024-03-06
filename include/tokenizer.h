#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>

#define CHAR_QUOTE '\''
#define CHAR_DQUOTE '"'
#define CHAR_VERTLINE '|'
#define CHAR_AMPERSAND '&'
#define CHAR_SEMICOLON ';'
#define CHAR_NEWLINE '\n'
#define CHAR_NULL '\0'
#define CHAR_SPACE ' '
#define CHAR_TAB '\t'
#define CHAR_GT '>'
#define CHAR_LT '<'
#define CHAR_DOLLAR '$'
#define CHAR_UNDERSCORE '_'
#define CHAR_QUESTION '?'

typedef enum
{
    WORD,
    PIPE,
    SEMICOLON,
    NEWLINE,
    HEREDOC,
    REDIR_IN,
    REDIR_OUT,
    DREDIR_OUT,
    ENV_VAR,
    QUOTE,
    DQUOTE,
    SPACE,
} e_token_type;

typedef enum
{
	NO_QUOTES,
	IN_QUOTES,
	IN_DQUOTES
} e_token_quotes;

typedef struct s_token
{
    e_token_type type;
    char *contents;
    int len;
    e_token_quotes quotes;
    struct s_token *prev;
    struct s_token *next;
} t_token;

t_token	*new_token(e_token_type type, char *contents, int len, e_token_quotes quotes);
t_token	*token_find(t_token *token_lst, e_token_type type);
void	token_add_back(t_token **token_lst, t_token *new);
void	token_free(t_token **token_lst);
void tokenize_env_var(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
void tokenize_quote(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
void tokenize_dquote(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
void tokenize_redir_in(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
void tokenize_redir_out(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
void tokenize_word(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
void tokenize_single(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes);
t_token *tokelize_line(char *line);
int	is_space(char c);
int is_num(char c);
int is_alpha(char c);

#endif