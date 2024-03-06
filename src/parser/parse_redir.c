#include "include/minishell.h"

/*
 * retrieves the destination (path or file descriptor) associated with a redirection.
 * It considers different cases, such as quotes (DOUBLE_QUOTE or QOUTE), environment variables (ENV), and regular words (WORD).
 *
*/
static char	*get_redir_dest(t_token **token, t_minishell *minishell)
{
	char	*dest;

	dest = NULL;
	if ((*token)->type == DQUOTE || (*token)->type == QOUTE)
		dest = parse_quo(dest, token, (*token)->type, minishell);
	else if ((*token)->type == ENV)
	{
		dest = get_env(minishell->enviro, (*token)->contents + 1);
		if (*token)
			*token = (*token)->next;
	}
	else if ((*token)->type == WORD)
	{
		dest = ft_strdup((*token)->contents);
		if (*token)
			*token = (*token)->next;
	}
	return (dest);
}

/*
 *
 * Function checks if a redirection is already set in a t__cmd structure and clears the associated data if needed. 
 * It's called before parsing a cmd redirection to avoid conflicts.
*/
static void	rm_redir_if_any(t_cmd *cmd, enum e_token_type type)
{
	if (cmd->in_redir && (type == REDIR_IN || type == HERE_DOC))
	{
		free(cmd->in_filename);
		cmd->in_filename = NULL;
		free(cmd->in_redir);
		cmd->in_redir = NULL;
	}
	else if ((cmd->out_redir && type == REDIR_OUT)
		|| (cmd->out_redir && type == DREDIR_OUT))
	{
		free(cmd->out_filename);
		cmd->out_filename = NULL;
		free(cmd->out_redir);
		cmd->out_redir = NULL;
	}
}


void	parse_redir(t_token **token, t_minishell *minishell, t_cmd *cmd)
{
	enum e_token_type	type;
	char			    *dest;

	type = (*token)->type;
	check_redir(cmd, type);
	if (type == REDIR_IN || type == DREDIR_OUT)
		cmd->out_redir = get_redir_symb(type);
	else
		cmd->in_redir = get_redir_symb(type);
	while ((*token)->type != WORD && (*token)->type != ENV
		&& (*token)->type != DQUOTE && (*token)->type != QOUTE)
		*token = (*token)->next;
	dest = get_redir_dest(token, minishell);
	if (type == REDIR_OUT || type == DREDIR_OUT)
		cmd->out_filename = dest;
	else
		cmd->in_filename = dest;
}