#include "include/minishell.h"


void parse_env_var(t_token **elem, char **args, int *i, t_minishell *minishell)
{
    char *tmp;

    if (ft_strncmp((*elem)->contents, "$?", ft_strlen("$?")) == 0)
    {
        // Special case: $?
        args[*i] = ft_itoa(minishell->exitstatus);
    }
    else
    {
        // Regular environment variable
        tmp = get_env(minishell->enviro, (*elem)->contents + 1);
        if (tmp == NULL)
        {
            // If the environment variable is not found, treat it as a regular word
            args[*i] = ft_strdup((*elem)->contents);
            if ((*elem)->next && (*elem)->next->type == WORD)
            {
                // If the next token is a WORD, concatenate it to the current argument
                tmp = args[*i];
                args[*i] = ft_strjoin(tmp, (*elem)->next->contents);
                free(tmp);
                (*elem) = (*elem)->next;
            }
        }
        else
        {
            // If the environment variable is found, store its value in the args array
            args[*i] = tmp;
        }
    }

    // Move to the next token in the list
    if (args[*i])
        (*i)++;
    (*elem) = (*elem)->next;
}


void parse_word(t_token **elem, char **args, int *i)
{
    // Extract the contents of the WORD token and store it in the args array
    args[*i] = ft_substr((*elem)->contents, 0, (*elem)->len + 1);
    (*i)++;
    
    // Move to the next token in the list
    (*elem) = (*elem)->next;
}

/*
 *
 * designed to parse the contents within quotes (QOUTE or DOUBLE_QUOTE). 
 * It appends the contents within the quotes to the line variable and handles special cases where the contents inside quotes includes environment variables.
 * The val variable is used to store the value of environment variables if they are present inside the quotes.
 * 
 * //type === QOUTE or DQUOTE
*/
char	*parse_in_quotes(char *line, t_token **token, enum e_token_type type, t_minishell *minishell)
{
	char	*val;
	char	*tmp;

	val = NULL;
	*token = (*token)->next;
	if ((*token) && (*token)->type == type)
	{
		if (!line)
			line = ft_strdup("");
		*token = (*token)->next;
		return (line);
	}
	while ((*token) && (*token)->type != type)
	{
		if (!line)
			line = ft_strdup("");
        tmp = NULL;
        if ((*token)->type == ENV && (*token)->state == IN_DQUOTE)
        {
            if (ft_strncmp((*token)->contents, "$?", ft_strlen("$?")) == 0)
                *val = ft_itoa(minishell->exitstatus);
            else
                *val = get_env(minishell->enviro, (*token)->contents + 1);
            if (*val)
            {
                tmp = ft_strjoin(*line, *val);
                free(*val);
                *val = NULL;
            }
            else
            {
                *val = ft_strjoin(*line, (*token)->contents);
                if ((*token)->next && (*token)->next->type == WORD)
                {
                    tmp = ft_strjoin(*val, (*token)->next->contents);
                    free(*val);
                    *val = NULL;
                    (*token) = (*token)->next;
                }
            }
        }
        else
            tmp = ft_strjoin(*line, (*token)->contents);
        free(*line);
        *line = tmp;
		*token = (*token)->next;
	}
	if (val)
		free(val);
	if (*token)
		*token = (*token)->next;
	return (line);
}
