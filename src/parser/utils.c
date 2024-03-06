#include "include/minishell.h"

int	is_alnum(char c)
{
	if ((c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z'))
		return (TRUE);
	return (FALSE);
}

int	is_space(char c)
{
	return (
		c == ' ' || c == '\t' || c == '\v'
		|| c == '\r' || c == '\f' || c == '\n'
		);
}

int	is_in_charset(char c)
{
	return (c == '\'' || c == '\"' || c == '<' || c == '>' || c == '|'
		|| c == '$' || ft_isspace(c) || c == 0);
}


/*prev name "ft_has_characters"*/
int is_any_printable(char *chars)
{
	int	i;

    if (!chars)
        return (FALSE);
	i = 0;
    while (chars[i])
    {
        if (chars[i] > 32 && chars[i] < 127)
            return (TRUE);
        i++;
    }
	return (FALSE);
}


/*
 *
 *  function returns a string representation of the redirection symbol associated with the given type.
 * It uses ft_strdup to create a duplicate of the string.
*/
char	*get_redir_symbol(enum e_token_type type)
{
	if (type == REDIR_IN)
		return (ft_strdup("<"));
	else if (type == REDIR_OUT)
		return (ft_strdup(">"));
	if (type == HERE_DOC)
		return (ft_strdup("<<"));
	if (type == DREDIR_OUT)
		return (ft_strdup(">>"));
	return (NULL);
}


int	is_redir(enum e_token_type type)
{
	return (type == REDIR_IN || type == REDIR_OUT
		|| type == DREDIR_OUT || type == HERE_DOC);
}