#include "../include/lexer_parser.h"


t_token	*new_token(char *type, char *contents, int len, enum e_token_state state)
{
	t_token	*new;

	new = (t_token *)malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->contents = contents;
	new->len = len;
	new->type = type;
	new->state = state;
	new->prev = NULL;
	new->next = NULL;
	return (new);
}

t_token	*token_find(t_token *token_lst, char *type)
{
    t_token	*curr;

    if (!token_lst)
        return (NULL);
    curr = token_lst;
    while (curr)
    {
        if (ft_strcmp(curr->type, type) == 0)
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
