#include "../include/minishell.h"


/*
 *
 * The waitpid system call is used to wait for a specific child process (pid) to terminate. 
 * The exit status of the process is stored in the status variable.
 * The macro WIFEXITED(status) checks whether the child process terminated normally (exited). 
 * If true (non-zero), the function returns the exit status using WEXITSTATUS(status).
 * If the child process did not terminate normally (e.g., it was terminated by a signal), 
 * the function returns a default exit status of 130. This value (130) is commonly used to indicate that the process was terminated 
 * by a user interrupt (e.g., pressing Ctrl+C), and it's a convention used in some shells.
 * 
*/
int	ft_waitpid(pid_t pid)
{
	int	status;

	// Wait for the specified process to terminate
	waitpid(pid, &status, 0);

	// Check if the process terminated normally (WIFEXITED)
	if (WIFEXITED(status) == 1)
		return (WEXITSTATUS(status));  // Return the exit status of the process

	// If the process did not terminate normally, return a default exit status (130)
	return (130);
}


/*
 *
 * function designed to handle a here document (heredoc) in the context of command execution. 
 * A heredoc allows input to be provided directly within the script or command, typically terminated by a specific delimiter. 
 * The function sets up a pipe, reads lines from the user, writes them to the write end of the pipe, and redirects the read end 
 * of the pipe to the input file descriptor of the command.
 * t creates a pipe using the pipe system call. The pipe has two ends: fd[0] (read end) and fd[1] (write end).
 * It enters an infinite loop to read lines from the user using readline.
 * Inside the loop:
 * If the user enters a null line or a line matching the specified terminator (cmd->in_name), the loop is terminated.
 * Otherwise, the function writes the line followed by a newline character to the write end of the pipe (fd[1]).
 * After the loop, it checks if line is not null (indicating a successful readline), and if so, frees the allocated memory for line.
 * It closes the write end of the pipe (fd[1]) and the original input file descriptor of the command (cmd->in_fd).
 * It assigns the read end of the pipe (fd[0]) to the input file descriptor of the command (cmd->in_fd), establishing the connection between the heredoc and the command.
 * 
*/
int	heredoc_function(t_cmd *cmd)
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
			if (line == NULL || (ft_strncmp(line, cmd->in_filename,
						ft_strlen(cmd->in_filename) + 1) == 0))
				break ;
			write(fd[1], line, ft_strlen(line));
			write(fd[1], "\n", 1);
		}
		if (line)
			free(line);
		ft_close_two_fd(&fd[1], &cmd->in_fd);
		cmd->in_fd = fd[0];
	}
	return (0);
}


static int	get_path(t_minishell *minishell, t_cmd *cmd)
{
	char	*tmp;
	int		i;

	i = 0;

	// Check if the command name is valid
	if (cmd->cmd_args[0] == NULL || ft_strlen(cmd->cmd_args[0]) == 0)
		return (FALSE);

	// Iterate through the directories in minishell->path
	while (minishell->exec_paths && minishell->exec_paths[i])
	{
		// Construct the full path for the executable
		tmp = ft_strjoin(minishell->exec_paths[i++], "/");
		cmd->exec_path = ft_strjoin(tmp, cmd->cmd_args[0]);
		free(tmp);

		// Check if the constructed path is executable
		if (access(cmd->exec_path, X_OK) == 0)
			return (TRUE);

		// Free the memory allocated for the current path
		free(cmd->exec_path);
		cmd->exec_path = NULL;
	}

	// Return FALSE if no valid path is found for the executable
	return (FALSE);
}

/*
 *
 * function is responsible for executing a command using the execve system call.
 * This function is a crucial part of command execution, as it replaces the current process image with a new one based on the specified command and arguments.
 * 
 * ADD MINISHELL free
*/
void	ft_execve(t_cmd *cmd, t_minishell *minishell, char **env)
{
	// If the path is not set and cannot be obtained, display an error and exit
	if (cmd->cmd_args == NULL && get_path(minishell, cmd) == FALSE)
	{
		ft_strerror("command not found", cmd->cmd_args[0]);
		ft_close_two_fd(&cmd->in_fd, &cmd->out_fd);
		exit(127);
	}

	// Execute the command using execve
	if (execve(cmd->exec_path, cmd->cmd_args, env) == -1)
	{
		// If execve fails, display an error and exit
		ft_close_two_fd(&cmd->in_fd, &cmd->out_fd);
		ft_strerror(strerror(errno), cmd->cmd_args[0]);
		exit(126);
	}
}


/*
 *
 * function is responsible for executing built-in commands in a child process. 
 * It checks the command name and calls the corresponding built-in function.
 * The purpose of forking a child process for built-ins is to isolate their execution and avoid affecting the main shell process.
 * The rest of the builtins - the ones that have output to console
 * 
*/
void	ft_execute_fork_builtin(t_minishell *minishell, t_cmd *cmd)
{
	// Check if the command is "echo"
	if (ft_strncmp(cmd->cmd_args[0], "echo", 5) == 0)
		ft_echo(cmd);
	// Check if the command is "pwd"
	else if (ft_strncmp(cmd->cmd_args[0], "pwd", 4) == 0)
		ft_pwd(cmd);
	// If the command is not "echo" or "pwd", assume it's "env"
	else
		ft_env(minishell);

	// Exit the child process with status 0
	exit(0);
}

/*
 * function that handles input and output redirection before executing the command in the child process.
*/
static int execute_cmd_ext(t_cmd *cmd, int is_builtin)
{
    // If input file descriptor is negative, duplicate standard input
    if (cmd->in_fd < 0)
        cmd->in_fd = dup(STDIN_FILENO);

    // If output file descriptor is negative, duplicate standard output
    if (cmd->out_fd < 0)
        cmd->out_fd = dup(STDOUT_FILENO);

    // Handle here document (heredoc) if redirection is "<<"
    if (cmd->in_redir && ft_strncmp(cmd->in_redir, "<<", ft_strlen("<<") + 1) == 0)
        heredoc_function(cmd);

    // If it's not a builtin command, perform input redirection
    if (is_builtin == FALSE)
    {
        if (dup2(cmd->in_fd, STDIN_FILENO) < 0)
            return (ft_strerror(strerror(errno), "dup2 error"), -2);
    }
    else
    {
        // If it's a builtin command, close the input file descriptor
        if (cmd->in_fd > 0)
            close(cmd->in_fd);
    }

    // Perform output redirection
    if (dup2(cmd->out_fd, STDOUT_FILENO) < 0)
        return (ft_strerror(strerror(errno), "dup2 error"), -3);

    return (0);
}

/*
 * function forks a child process using fork() and then executes the command in the child process.
 * signal handler for the SIGINT signal in the child process. This means that when the child process receives the SIGINT signal 
 * (which is typically triggered by pressing Ctrl+C in the terminal), the sig_handl_child function will be called.
 * signal(SIGINT, SIG_IGN); line is used to ignore the SIGINT signal in the current process. 
 * This means that if the process receives a SIGINT signal (typically sent when the user presses Ctrl+C in the terminal),
 * the default action of terminating the process will be ignored, and the process will continue running.
*/
int execute_cmd(t_cmd *cmd, t_minishell *minishell, char **env)
{
    pid_t pid;
    int is_builtin;
    int exit_status;

    // Check if the command is a builtin command that produces output
    is_builtin = is_builtin_with_output(cmd->cmd_args[0]);

    // Fork a child process
    pid = fork();
    if (pid < 0)
        return (ft_strerror(strerror(errno), "fork"), -1);
    else if (pid == 0)
    {
        // Child process
        signal(SIGINT, exit_on_interrupt);

        // Execute the command in the child process
        if (execute_cmd_ext(cmd, is_builtin) != 0)
            return (-1);

        // If it's a builtin command, execute it in the child process
        if (is_builtin)
            ft_execute_fork_builtin(minishell, cmd);

        // Execute the command using execve
        ft_execve(cmd, minishell, env);
    }

    // Parent process
    signal(SIGINT, SIG_IGN);

    // Close the input and output file descriptors in the parent process
    ft_close_two_fd(&cmd->in_fd, &cmd->out_fd);

    // Wait for the child process to finish and get the exit status
    exit_status = ft_waitpid(pid);

    // Restore the signal handler for SIGINT in the parent process
    signal(SIGINT, clear_input_on_interrupt);

    return (exit_status);
}