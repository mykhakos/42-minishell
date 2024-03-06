#include "../include/minishell.h"

int	create_pipes(t_minishell	*minishell)
{
	int	i;

	i = 0;
	if (minishell->nbrofcom > 1)
		minishell->fd_pipes = (int *)malloc(sizeof(int) * minishell->nbr_pipes * 2);
	while (i < minishell->nbr_pipes * 2)
	{
		if (pipe(minishell->fd_pipes + i) == -1) // from unistd.h
			return (ft_strerror("Pipes creating", "error"), -1);
		i += 2;
	}
	return (0);
}


void close_pipes(t_minishell *minishell)
{
    int i;

    i = 0;

    // Iterate through the array of pipe-related file descriptors
    while (i < minishell->nbr_pipes * 2)
    {
        // Close the read and write ends of the pipe
        ft_close_two_fd(&minishell->fd_pipes[i], &minishell->fd_pipes[i + 1]);

        // Move to the next pair of file descriptors
        i += 2;
    }
}


int	ft_execute_builtin(t_minishell *minishell, t_cmd *cmd)
{
	if (cmd->in_fd > 0)
		close(cmd->in_fd);
	if (cmd->out_fd > 0)
		close(cmd->out_fd);
	if (ft_strncmp(cmd->cmd_args[0], "cd", 3) == 0)
		return ft_cd(minishell, cmd);
	if (ft_strncmp(cmd->cmd_args[0], "export", 7) == 0)
		return ft_export(minishell, cmd);
	if (ft_strncmp(cmd->cmd_args[0], "unset", 6) == 0)
		return ft_unset(minishell, cmd);
	if (ft_strncmp(cmd->cmd_args[0], "exit", 5) == 0)
		return ft_exit(minishell, cmd);
}


static int	ft_executor_ext_one(t_minishell *minishell)
{
	if (create_pipes(minishell) == -1)
	{
		minishell->exitstatus = 1;
		return (-1);
	}
	redirect_inputs_outputs(minishell);
	assign_fds(minishell);
	return (0);
}

static void	ft_executor_ext_two(t_minishell *minishell, t_cmd *cmd, char **envp)
{
	if (ft_is_builtin(cmd->cmd_args[0]) && !is_builtin_with_output(cmd->cmd_args[0]))
		minishell->exitstatus = ft_execute_builtin(minishell, cmd);
	else
		minishell->exitstatus = execute_cmd(cmd, minishell, envp);
	signal(SIGINT, clear_input_on_interrupt);
}


void executor(t_minishell *minishell, char **envp)
{
    t_cmd *cmd;

    // Check if preparation for execution (pipes and redirections) was successful
    if (ft_executor_ext_one(minishell) != 0)
        return;

    // Iterate through the linked list of command nodes
    cmd = minishell->cmds;
    while (cmd)
    {
        // Check for conditions where the command execution should be skipped
        if (!cmd->cmd_args[0] || (cmd->cmd_args[0] && cmd->in_fd == -1))
        {
            // Handle special case for HEREDOC error with an empty command
            if (!cmd->cmd_args[0] && cmd->in_redir
                && ft_strncmp(cmd->in_redir, "<<", ft_strlen("<<") + 1) == 0)
            {
                ft_strerror("HEREDOC error", "empty command");
                minishell->exitstatus = 1;
            }

            // Close file descriptors and move to the next command node
            ft_close_two_fd(&cmd->in_fd, &cmd->out_fd);
            cmd = cmd->next;
            continue;
        }

        // Execute the command and handle redirections
        ft_executor_ext_two(minishell, cmd, envp);

        // Move to the next command node
        cmd = cmd->next;
    }

    // Close any remaining pipes
    close_pipes(minishell);
}