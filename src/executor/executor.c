#include "../include/minishell.h"

/*
 * function that handles input and output redirection before executing the command in the child process.
*/
static int ft_in_out_handler(t_cmd *node, int is_builtin_out)
{
	if (node->in_fd < 0)
		node->in_fd = dup(STDIN_FILENO);

	if (node->out_fd < 0)
		node->out_fd = dup(STDOUT_FILENO);

	if (node->redir_in && ft_strncmp(node->redir_in, "<<", ft_strlen("<<") + 1) == 0)
		heredoc_function(node);

	if (is_builtin_out == FALSE)
	{
		if (dup2(node->in_fd, STDIN_FILENO) < 0)
			return (ft_strerror(strerror(errno), "dup2 error"), -2);
	}
	else
	{
		if (node->in_fd > 0)
		close(node->in_fd);
	}
	if (dup2(node->out_fd, STDOUT_FILENO) < 0)
		return (ft_strerror(strerror(errno), "dup2 error"), -3);
	return (0);
}

int execute_cmd(t_cmd *node, t_minishell *mini, char **env)
{
    pid_t pid;
    int is_builtin_out;
    int exit_status;

	is_builtin_out = ft_is_builtin_with_output(node->cmd[0]);

	pid = fork();
	if (pid < 0)
		return (ft_strerror(strerror(errno), "fork"), -1);
	else if (pid == 0)
	{
		signal(SIGINT, exit_on_interrupt);

		if (ft_in_out_handler(node, is_builtin_out) != 0)
			return (-1);
		if (is_builtin_out)
			ft_execute_fork_builtin(mini, node);

		ft_execve(node, mini, env);
	}

	signal(SIGINT, SIG_IGN);
	ft_close_two_fd(&node->in_fd, &node->out_fd);
	exit_status = ft_waitpid(pid);
	signal(SIGINT, clear_input_on_interrupt);
	return (exit_status);
}

static void	ft_exe_ext_or_build(t_minishell *mini, t_cmd *tmp, char **envp)
{
	if (ft_is_builtin(tmp->cmd[0]) && !ft_is_builtin_with_output(tmp->cmd[0]))
		mini->exit_status = ft_execute_builtin(mini, tmp);
	else
		mini->exit_status = execute_cmd(tmp, mini, envp);
	signal(SIGINT, clear_input_on_interrupt);
}

static int	ft_prep_for_exec(t_minishell *mini)
{
	if (create_pipes(mini) == -1)
	{
		mini->exit_status = 1;
		return (-1);
	}
	ft_redirect_inputs_outputs(mini);
	assign_fds(mini);

	return (0);
}

void ft_executor(t_minishell *mini, char **envp)
{
	t_cmd *tmp;

	if (ft_prep_for_exec(mini) != 0)
		return;
	tmp = mini->cmd_lst;
	while (tmp)
	{
		if (!tmp->cmd[0] || (tmp->cmd[0] && tmp->in_fd == -1))
        {
            // Handle special case for HEREDOC error with an empty command
            if (!tmp->cmd[0] && tmp->redir_in
                && ft_strncmp(tmp->redir_in, "<<", ft_strlen("<<") + 1) == 0)
            {
                ft_strerror("HEREDOC error", "empty command");
                mini->exit_status = 1;
            }

            // Close file descriptors and move to the next command node
            ft_close_two_fd(&tmp->in_fd, &tmp->out_fd);
            tmp = tmp->next;
            continue;
        }

        // Execute the command and handle redirections
        ft_exe_ext_or_build(mini, tmp, envp);	// execute builtin without outpu

        // Move to the next command node
        tmp = tmp->next;
    }
    ft_close_pipes(mini);
}
