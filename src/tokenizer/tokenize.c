#include "../include/tokenizer.h"


static void tokenize_dollar(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;

    new = new_token(WORD, str + *str_idx, 1, *quotes);
    token_add_back(token_lst, new);
    *str_idx++;
}

static void tokenize_other(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;
    char c;
    
    c = str[*str_idx];
    if (c == CHAR_VERTLINE)
        new = new_token(PIPE, str + *str_idx, 1, *quotes);
    else if (is_space(c))
        new = new_token(SPACE, str + *str_idx, 1, *quotes);
    token_add_back(token_lst, new);
    *str_idx++;
}

void tokenize_single(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    char c;
    
    c = str[*str_idx];
    if (c && !is_in_charset(c, "\'\"<>|$") && !is_space(c))
        tokenize_word(token_lst, str, str_idx, quotes);
    else if (c == CHAR_QUOTE)
        tokenize_quote(token_lst, str, str_idx, quotes);
    else if (c == CHAR_DQUOTE)
        tokenize_dquote(token_lst, str, str_idx, quotes);
    else if (c == CHAR_DOLLAR)
    {
        if (str[*str_idx + 1] && is_in_charset(str[*str_idx + 1], "\'\"<>|$"))
            tokenize_dollar(token_lst, str, str_idx, quotes);
        else
            tokenize_env_var(token_lst, str, str_idx, quotes);
    }
    else if (c == CHAR_LT)
        tokenize_redir_in(token_lst, str, str_idx, quotes);
    else if (c == CHAR_GT)
        tokenize_redir_out(token_lst, str, str_idx, quotes);
    else 
        tokenize_other(token_lst, str, str_idx, quotes);
}


t_token *tokelize_line(char *line)
{
    t_token *tokens;
    e_token_quotes quotes;
    int i;

    tokens = NULL;
    quotes = NO_QUOTES;
    i = 0;
    while (line[i])
        tokenize_single(&tokens, line, &i, &quotes);
    return (tokens);
}
