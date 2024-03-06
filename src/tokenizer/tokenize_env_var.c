#include "../include/tokenizer.h"


void tokenize_env_var(t_token **token_lst, char *str, int *str_idx, e_token_quotes *quotes)
{
    t_token *new;
    int i;

    i = *str_idx + 1;
    if (str[i] == CHAR_QUESTION || is_num(str[i]))
        i++;
    else
    {
        while (str[i] && (is_num(str[i]) || is_alpha(str[i])))
            i++;
    }
    new = new_token(ENV_VAR, str + *str_idx, i - *str_idx, *quotes);
	token_add_back(token_lst, new);
    *str_idx = i;
}
