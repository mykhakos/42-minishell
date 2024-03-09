#include "../include/minishell.h"

void	ft_execve(t_cmd *node, t_minishell *mini, char **env)
{
	if (node->exec_path == NULL && check_path(mini, node) == FALSE)
	{
		ft_strerror("command not found", node->cmd_args[0]);
		ft_close_two_fd(&node->in_fd, &node->out_fd);
		exit(127);
	}
	if (execve(node->exec_path, node->cmd_args, env) == -1)
	{
		ft_close_two_fd(&node->in_fd, &node->out_fd);
		ft_strerror(strerror(errno), node->cmd_args[0]);
		exit(126);
	}
}

int	ft_execute_builtin(t_minishell *mini, t_cmd *cmd)
{
	int	res;

	res = 0;
	if (cmd->in_fd > 0)
		close(cmd->in_fd);
	if (cmd->out_fd > 0)
		close(cmd->out_fd);
	if (ft_strncmp(cmd->cmd_args[0], "cd", 3) == 0)
		res = ft_cd(mini, cmd);
	else if (ft_strncmp(cmd->cmd_args[0], "export", 7) == 0)
		res = ft_export(mini, cmd);
	else if (ft_strncmp(cmd->cmd_args[0], "unset", 6) == 0)
		res = ft_unset(mini, cmd);
	else if (ft_strncmp(cmd->cmd_args[0], "exit", 5) == 0)
		res = ft_exit(mini, cmd);
	return (res);
}

void	ft_execute_fork_builtin(t_minishell *mini, t_cmd *cmd)
{
	if (ft_strncmp(cmd->cmd_args[0], "echo", 5) == 0)
		ft_echo(cmd);
	else if (ft_strncmp(cmd->cmd_args[0], "pwd", 4) == 0)
		ft_pwd(cmd);
	else
		ft_env(mini);
	exit(0);
}
