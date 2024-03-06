#include "include/minishell.h"


/*
 *  Validates the command prefix of an environment variable.
 *  The prefix must start with an alphabetic character or an underscore.
 *  The prefix must contain at least one '=' character.
 *  Returns TRUE if the prefix is valid, FALSE otherwise.
*/
static int	validate_cmd_prefix(char *cmd)
{
	int	i;

	i = 0;
	if (ft_strlen(cmd) <= 1)
	{
		ft_strerror(cmd, "invalid prefix");
		return (FALSE);
	}
	if (!ft_isalpha(cmd[0]) && cmd[0] != UNDERSCORE)
	{
		ft_strerror(cmd, "invalid prefix");
		return (FALSE);
	}
	while (cmd[i])
	{
		if (cmd[i] == EQUALS)
			return (TRUE);
		i++;
	}
	ft_strerror(cmd, "invalid prefix");
	return (FALSE);
}


static int	update_env_var(t_env_var *env, t_cmd *cmd)
{
	t_node	*curr;
	char	*temp;

	curr = env;
	temp = extract_key(cmd->cmd_args[1]);
	while (curr)
	{
		if (ft_strncmp(curr->key, temp, ft_strlen(temp) + 1) == 0)
		{
			free(temp);
			free(curr->value);
			curr->value = extract_value(cmd->cmd_args[1]);
			return (TRUE);
		}
		curr = curr->next;
	}
	free(temp);
	return (FALSE);
}

/*
 *  Handles the export cmdand, adding or updating environment variables.
 *  Calls ex_cmdand_tester to validate the cmdand.
 *  If the cmdand is valid, either updates an existing environment variable or adds a new one.
 *  Cleans the minishell->path array, then refills it with the updated environment variables.
 *  Prints the exported environment variables.
*/
int	ft_export(t_minishell *minishell, t_cmd *cmd)
{
	t_node	*curr;

	if (cmd->cmd_args[1])
	{
		if (!validate_cmd_prefix(cmd->cmd_args[1]))
			return (2);
		if (!update_env_var(&minishell->enviro, cmd))
			ft_add_end(&minishell->enviro, cmd->cmd_args[1]);
		path_clean(minishell);
		update_path(minishell);
		return (0);
	}
	curr = minishell->enviro;
	while (curr != NULL)
	{
		if (ft_strncmp(curr->key, "LINES", 6) != 0
			|| ft_strncmp(curr->key, "COLUMNS", 8) != 0)
			printf("declare -x %s%c\"%s\"\n", curr->key, curr->eq, curr->value);
		curr = curr->next;
	}
	return (0);
}