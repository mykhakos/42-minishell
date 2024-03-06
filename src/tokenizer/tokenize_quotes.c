#include "../include/tokenizer.h"


void tokenize_quote(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;

    if (*quotes == NO_QUOTES)
    {
        new = new_token(QUOTE, str + *str_idx, 1, NO_QUOTES);
        *quotes = IN_QUOTES;
    }
    else if (*quotes == IN_QUOTES)
    {
        new = new_token(QUOTE, str + *str_idx, 1, NO_QUOTES);
        *quotes = NO_QUOTES;
    }
    else
    {
        new = new_token(QUOTE, str + *str_idx, 1, IN_DQUOTES);
    }
    token_add_back(token_lst, new);
    *str_idx++;
}

void tokenize_dquote(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;

    if (*quotes == NO_QUOTES)
    {
        new = new_token(DQUOTE, str + *str_idx, 1, NO_QUOTES);
        *quotes = IN_DQUOTES;
    }
    else if (*quotes == IN_DQUOTES)
    {
        new = new_token(DQUOTE, str + *str_idx, 1, NO_QUOTES);
        *quotes = NO_QUOTES;
    }
    else
    {
        new = new_token(DQUOTE, str + *str_idx, 1, IN_QUOTES);
    }
    token_add_back(token_lst, new);
    *str_idx++;
}
