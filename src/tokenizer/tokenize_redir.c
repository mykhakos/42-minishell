#include "../include/tokenizer.h"


void tokenize_redir_in(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;

    if (str[*str_idx + 1] == CHAR_LT)
    {
        new = new_token(HEREDOC, str + *str_idx, 2, *quotes);
        *str_idx += 2;
    }
    else
    {
        new = new_token(REDIR_IN, str + *str_idx, 1, *quotes);
        *str_idx += 1;
    }
    token_add_back(token_lst, new);
}

void tokenize_redir_out(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;

    if (str[*str_idx + 1] == CHAR_GT)
    {
        new = new_token(DREDIR_OUT, str + *str_idx, 2, *quotes);
        *str_idx += 2;
    }
    else
    {
        new = new_token(REDIR_OUT, str + *str_idx, 1, *quotes);
        *str_idx += 1;
    }
    token_add_back(token_lst, new);
}
