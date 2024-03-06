#include "include/minishell.h"


/*
 * Removes an environment variable specified in the command.
 * Calls ft_remove_node to remove the corresponding node from the linked list.
 * Clears the minishell->path array and refills it with the updated environment variables.
*/
int	ft_unset(t_minishell *minishell, t_cmd *cmd)
{
	t_node	*current;
	int		i;

	i = 0;
	if (!cmd->cmd_args[1])
		return (0);
	current = minishell->enviro;
	while (current != NULL)
	{
		if (ft_strncmp(current->key, cmd->cmd_args[1], ft_strlen(cmd->cmd_args[1])) == 0)
		{
			ft_remove_node(&minishell->enviro, cmd->cmd_args[1]);
			if (minishell->path != NULL)
			{
				while (minishell->path[i])
					free(minishell->path[i++]);
				free(minishell->path);
				minishell->path = NULL;
			}
			update_path(minishell);
			return (0);
		}
		else
			current = current->next;
	}
	return (0);
}