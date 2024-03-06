#include "../include/tokenizer.h"


t_token	*new_token(e_token_type type, char *contents, int len, e_token_quotes quotes)
{
	t_token	*new;

	new = (t_token *)malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->type = type;
	new->contents = contents;
	new->len = len;
	new->quotes = quotes;
	new->prev = NULL;
	new->next = NULL;
	return (new);
}

t_token	*token_find(t_token *token_lst, e_token_type type)
{
    t_token	*curr;

    if (!token_lst)
        return (NULL);
    curr = token_lst;
    while (curr)
    {
        if (curr->type == type)
            return (curr);
        curr = curr->next;
    }
    return (NULL);
}

void	token_add_back(t_token **token_lst, t_token *new)
{
	t_token	*curr;

	if (!token_lst || !new)
		return ;
	if (!(*token_lst))
	{
		*token_lst = new;
		return ;
	}
	curr = *token_lst;
	while (curr->next)
		curr = curr->next;
	curr->next = new;
	new->prev = curr;
}

void	token_free(t_token **token_lst)
{
    t_token	*curr;
	t_token	*next;
	
	if (!token_lst || !(*token_lst))
		return ;
	curr = *token_lst;
	while (curr)
	{
		next = curr->next;
        free(curr->contents);
		free(curr);
		curr = next;
	}
	free(*token_lst);
}
