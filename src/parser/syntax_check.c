#include "include/minishell.h"


/*
 *
 * ptr is a pointer to a token (t_elem) representing the current position in the token list.
 * opt is an integer parameter that determines the direction in which to skip space. If opt is non-zero (true), it means skipping forward (using ptr->next), otherwise, it means skipping backward (using ptr->prev).
 * The function uses a while loop to iterate through consecutive white space tokens. Inside the loop:
 * If ptr is not NULL and the type of the token is WHITE_SPACE, it continues to skip over white space.
 * If opt is non-zero, it moves ptr to the next token (ptr->next), else it moves ptr to the previous token (ptr->prev).
 * The loop continues until a non-white space token is encountered or ptr becomes NULL.
 * Finally, the function returns the updated position of the pointer in the token list after skipping over white space tokens.
 * This updated pointer can be used by the caller to access the next non-white space token in the specified direction.
*/
t_token	*skip_space(t_token *ptr, int opt)  // change opt to something more descriptive
{
	while (ptr && ptr->type == WHITESPACE)
	{
		if (opt)
			ptr = ptr->next;
		else
			ptr = ptr->prev;
	}
	return (ptr);
}

/*
 *
 * This function checks for syntax errors related to pipes (|).
 * It looks at the previous and next tokens, skipping any leading/trailing spaces using the skip_space function.
 * If either the previous or next token is missing, it returns 1 (indicating an error)
*/
int	pipe_err(t_token *token)
{
	if (skip_space(token->prev, 0) && skip_space(token->next, 1))
		return (EXIT_SUCCESS);
	return (EXIT_FAILURE);
}

/*
 *
 * This function checks for syntax errors in redirections (e.g., <, >, >>).
 * It looks at the next token after the redirection symbol and ensures it is of a valid type (WORD, ENV, DOUBLE_QUOTE, or QOUTE).
*/
int	redir_err(t_token *token)
{
	t_token	*next;

	next = skip_space(token->next, 1);
	if (next && (next->type == WORD || next->type == ENV
			|| next->type == DQUOTE || next->type == QOUTE))
		return (EXIT_SUCCESS);
	return (EXIT_FAILURE);
}

/*
 *
 * function checks for unclosed quotes (single or double quotes).
 * It iterates through the tokens until it finds a token of the specified type or reaches the end of the list.
 * If it doesn't find the closing quote, it prints an error message and returns the position where the unclosed quotes were detected.
 * Inside the loop:
 * If *ptr is NULL or the type of the token is equal to the specified type (which represents the type of the closing quote), the loop breaks.
 * After the loop, if *ptr is still NULL, it means the closing quote was not found, and an error message is written to the standard error
*/
t_token	*quotes_err(t_token **token, enum e_token_type quote_type)
{
	while (*token)
	{
		*token = (*token)->next; // this is bad! we are modifying the token pointer silently
		if (!*token || (*token)->type == quote_type)
			break ;
	}
	if (!*token)
		write(2, "unclosed quotes detected.\n",
			ft_strlen("unclosed quotes detected.\n"));
	return (*token);
}


/*
 *
 * iterates through the linked list of tokens (t_elem) using a temporary pointer tmp.
 * For each token, it checks for syntax errors related to pipes, redirections, and quotes.
 * If a syntax error is found, it returns an error message using the ft_perror function.
 */
int check_syntax(t_token *token)
{
    t_token *temp;
	enum e_token_type type;

    temp = token;
    while (temp)
    {
		type = temp->type;
        if (type == PIPE_LINE && pipe_err(temp))
            return (ft_perror("Syntax error: unexpected token near ", "|"));
        if (is_redir(type) && redir_err(temp))
            return (ft_perror("Syntax error in redirection ", get_redir_symb(type)));
        if ((type == DQUOTE || type == QOUTE) && !quotes_err(&temp, type))
            return (ft_perror("Syntax error in quotes", NULL));
        temp = temp->next;
	}
	return (0);
}
