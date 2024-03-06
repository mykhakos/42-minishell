#include "../include/tokenizer.h"

void tokenize_word(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;
    int i;

    i = *str_idx;
    while (!is_in_charset(str[i], "\'\"<>|$") || is_space(str[i]))
        i++;
    new = new_token(WORD, str + *str_idx, i - *str_idx, *quotes);
    token_add_back(token_lst, new);
    *str_idx = i;
}
