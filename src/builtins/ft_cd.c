#include "include/minishell.h"


/*
 * Processes the command argument for cd and returns the updated path. 
 * It handles relative paths such as ".", "..", "./", and "../" by modifying the current working directory accordingly.
*/
char *dot_ext(char *pcwd, t_cmd *cmd)
{
	char *path[2];
	char *tmp;

	// Initialize variables
	path[0] = NULL;
	path[1] = NULL;
	tmp = pcwd;

	// Check if the command argument is ".", "./", or "../"
	if (!ft_strncmp(cmd->cmd_args[1], ".", 2) || !ft_strncmp(cmd->cmd_args[1], "./", 3))
		return (ft_strdup(pcwd));
	else if (ft_strncmp(cmd->cmd_args[1], "..", 3) == 0 || ft_strncmp(cmd->cmd_args[1], "../", 4) == 0)
		return (ft_substr(pcwd, 0, ft_strrchr(pcwd, '/') - pcwd));

	// Handle cases where the command argument starts with "../"
	path[0] = ft_strdup(pcwd);
	while (ft_strncmp(cmd->cmd_args[1], "../", 3) == 0)
	{
		// Update path to parent directory
		path[1] = ft_substr(path[0], 0, ft_strrchr(path[0], '/') - path[0]);
		free(path[0]);
		path[0] = path[1];

		// Update command argument to remove "../"
		tmp = ft_substr(cmd->cmd_args[1], 3, ft_strlen(cmd->cmd_args[1]) - 3);
		free(cmd->cmd_args[1]);
		cmd->cmd_args[1] = tmp;
	}

	// Combine path components
	path[1] = ft_strjoin(path[0], "/");
	free(path[0]);
	tmp = ft_strjoin(path[1], cmd->cmd_args[1]);

	// Clean up and return the updated path
	return (free(path[1]), tmp);
}

/*
 *  Handles special cases for constructing the path argument for the cd command.
 *  Calls dot_ext when the argument starts with a dot ('.').
 *  Combines the home directory with the specified path in cmd->cmd_args[1] and returns the result.
*/
char	*get_arg_cd_ext(char *home, char *pcwd, t_cmd *cmd, char c)
{
	char	*res;
	char	*tmp;

	tmp = NULL;
	res = NULL;
	if (c == '.')
		res = dot_ext(pcwd, cmd);
	else
	{
		if (ft_strlen(cmd->cmd_args[1]) == 1)
			return (ft_strdup(home));
		if (cmd->cmd_args[1][1] == '/')
			res = ft_strjoin(home, cmd->cmd_args[1] + 1);
		else
		{
			tmp = ft_strjoin(home, "/");
			res = ft_strjoin(tmp, cmd->cmd_args[1]);
			free(tmp);
		}
	}
	return (res);
}

/*
 *  Constructs the path argument for the cd command based on different cases.
 *  If no argument is provided, returns the home directory.
 *  If the argument is "-", returns the previous working directory.
 *  Handles cases where the argument starts with "~" or ".", calling get_arg_cd_ext accordingly.
 *  Otherwise, returns the specified argument.
*/
char	*get_arg_cd(t_cmd *cmd, char *home, char *old_cwd, char *pcwd)
{
	char	*res;

	res = NULL;
	if (cmd->cmd_args[1] == NULL)
	{
		if (home == NULL)
			return (res);
		res = ft_strdup(home);
	}
	else if (cmd->cmd_args[1] && ft_strncmp(cmd->cmd_args[1], "-", 2) == 0)
		return (ft_strdup(old_cwd));
	else if (cmd->cmd_args[1] && (cmd->cmd_args[1][0] == '~' || cmd->cmd_args[1][0] == '.'))
		res = get_arg_cd_ext(home, pcwd, cmd, cmd->cmd_args[1][0]);
	else
		res = ft_strdup(cmd->cmd_args[1]);
	return (res);
}


/*
 * Updates the values of environment variables "PWD" and "OLDPWD" 
 * based on the current working directory (cwd) and previous working directory (pcwd).
*/
static void	update_cwd(t_minishell *minishell, char *cwd, char *pcwd)
{
	t_env_var	*current;

	current = minishell->enviro;
	while (current != NULL)
	{
		if (ft_strncmp(current->key, "PWD", 4) == 0)
		{
			free(current->value);
			current->value = ft_strdup(cwd);
		}
		if (ft_strncmp(current->key, "OLDPWD", 7) == 0)
		{
			free(current->value);
			current->value = ft_strdup(pcwd);
		}
		current = current->next;
	}
}

/*
 *  Changes the current working directory.
 *  Handles various cases, such as changing to the home directory, the previous directory, or a specified directory.
 *  Updates the minishell->old_cwd and minishell->cwd accordingly.
 */
int	ft_cd(t_minishell *minishell, t_cmd *cmd)
{
	char	cwd[1024];
	char	pcwd[1024];
	char	*dest;

	if (cmd->cmd_args[1] && cmd->cmd_args[2] != NULL)
	{
		ft_strerror("too many arguments", cmd->cmd_args[0]);
		return (1);
	}
	getcwd(pcwd, 1024);
	dest = get_arg_cd(cmd, minishell->home, minishell->old_cwd, pcwd);
	if (chdir(dest) == -1)
	{
		ft_strerror(strerror(errno), dest);
		free(dest);
		return (1);
	}
	getcwd(cwd, 1024);
	free(minishell->old_cwd);
	minishell->old_cwd = ft_strdup(pcwd);
	update_cwd(minishell, cwd, pcwd);
	free(dest);
	return (0);
}
