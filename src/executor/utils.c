#include "../include/executor.h"

int	heredoc_function(t_cmd *node)
{
	int		fd[2];
	char	*line;

	if (pipe(fd) == -1)
		return (ft_strerror(strerror(errno), "heredoc pipe"), -1);
	else
	{
		while (1)
		{
			line = readline("heredoc> ");
			if (line == NULL || (ft_strncmp(line, node->in_name,
						ft_strlen(node->in_name) + 1) == 0))
				break ;
			write(fd[1], line, ft_strlen(line));
			write(fd[1], "\n", 1);
		}
		if (line)
			free(line);
		ft_close_two_fd(&fd[1], &node->in_fd);
		node->in_fd = fd[0];
	}
	return (0);
}

static int	check_path(t_minishell *mini, t_cmd *node)
{
	char	*tmp;
	int		i;
	int		res;

	res = 0;
	i = 0;
	if (node->cmd[0] == NULL || ft_strlen(node->cmd[0]) == 0)
		return (FALSE);
	while (mini->exec_paths && mini->exec_paths[i])
	{
		tmp = ft_strjoin(mini->exec_paths[i++], "/");
		node->path = ft_strjoin(tmp, node->cmd[0]);
		free(tmp);
		res = access(node->path, X_OK);
		if (res == 0)
			return (TRUE);
		free(node->path);
		node->path = NULL;
	}
	return (FALSE);
}

int	ft_waitpid(pid_t pid)
{
	int	status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) == 1)
		return (WEXITSTATUS(status));
	return (130);
}

int	ft_is_builtin_with_output(char *str)
{
	return (
        ft_strncmp(str, "echo", 5) == 0 ||
        ft_strncmp(str, "pwd", 4) == 0 ||
        ft_strncmp(str, "env", 4) == 0
        );
}

int	ft_is_builtin(char *str)
{
	return (
        ft_is_builtin_with_output(str) ||
        ft_strncmp(str, "cd", 3) == 0 ||
        ft_strncmp(str, "export", 7) == 0 ||
        ft_strncmp(str, "unset", 6) == 0 ||
        ft_strncmp(str, "exit", 5) == 0
        );
}