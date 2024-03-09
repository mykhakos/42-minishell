
#include "../include/minishell.h"

/*
 *
 * function checks whether each file descriptor (fd1 and fd2) is greater than 0, indicating that it is a valid file descriptor. 
 * If so, it closes the file descriptor using the close system call and updates the value to -2 or -3, depending on whether it's the first or second file descriptor.
 * The use of these specific values (-2 and -3) after closing the file descriptors might be indicative of some special meaning or state in the context of the program's design.
 * It's common in Unix-like systems to use negative values to represent certain states or flags.
 * 
*/
void	ft_close_two_fd(int *fd1, int *fd2)
{
	// If fd1 is a valid file descriptor, close it and update to -2
	if (*fd1 > 0)
	{
		close(*fd1);
		*fd1 = -2;
	}

	// If fd2 is a valid file descriptor, close it and update to -3
	if (*fd2 > 0)
	{
		close(*fd2);
		*fd2 = -3;
	}
}

/*
 * part executor
 * Function closes all the file descriptors associated with pipes in the mini structure
 * The function uses a loop to iterate through the array mini->pipe_fd_arr, which contains file descriptors associated with pipes.
 * For each iteration, it calls the ft_close_two_fd function to close the read and write ends of the pipe. This is done by passing the addresses of the current pair of file descriptors.
 * The loop increments i by 2 in each iteration to move to the next pair of file descriptors in the array.
*/
void ft_close_pipes(t_minishell *mini)
{
	int i;

	i = 0;

    // Iterate through the array of pipe-related file descriptors
    while (i < mini->pipe_count * 2)
    {
        // Close the read and write ends of the pipe
        ft_close_two_fd(&mini->pipe_fd_arr[i], &mini->pipe_fd_arr[i + 1]);

        // Move to the next pair of file descriptors
        i += 2;
    }
}

/*
 * part executor
 * function is responsible for creating pipes needed for inter-process communication between commands.
 * It allocates an array of file descriptors (mini->pipe_fd_arr) and initializes them using the pipe system call.
 * checks if there is more than one command (mini->cmd_count > 1) and dynamically allocates space for the array of file descriptors.
 *  It then uses a loop to create pipes using the pipe system call, updating the array of file descriptors accordingly.
 *
*/
int	create_pipes(t_minishell	*mini)
{
	int	i;

	i = 0;
	if (mini->cmd_count > 1)
		mini->pipe_fd_arr = (int *)malloc(sizeof(int) * mini->pipe_count * 2);
	while (i < mini->pipe_count * 2)
	{
		if (pipe(mini->pipe_fd_arr + i) == -1) // from unistd.h
			return (ft_strerror("Pipes creating", "error"), -1);
		i += 2;
	}
	return (0);
}


static void assign_fds_ext(t_minishell *mini, t_cmd *tmp, int *flag)
{
    if (tmp->out_fd == -3)
    {
        // Output should be connected to a pipe
        tmp->out_fd = mini->pipe_fd_arr[1]; 	// assigns mini->pipe_fd_arr[1] (write end of the pipe) to tmp->out_fd.
    }
    else
    {
        // Output is redirected elsewhere, close the write end of the pipe
        close(mini->pipe_fd_arr[1]); 
        mini->pipe_fd_arr[1] = -2; // Indicate that the pipe is not needed
    }

    *flag = FALSE;
}

static void assign_fds(t_minishell *mini)
{
    t_cmd *tmp;
    int first;
    int i;

    i = 0;
    tmp = mini->cmd_lst;
    first = TRUE;

    while (tmp)
    {
        // For the first command with a next command, handle output file descriptor
	if (first == TRUE && tmp->next)
            assign_fds_ext(mini, tmp, &first);
        // For the last command, if input file descriptor is unassigned, connect it to the last pipe
		else if (first == FALSE && tmp->next == NULL && tmp->in_fd == -2)
            tmp->in_fd = mini->pipe_fd_arr[mini->pipe_count * 2 - 2];
        // For subsequent commands, assign input and output file descriptors based on pipes
        else if (first == FALSE && tmp->next)
        {
            // If input file descriptor is unassigned, connect it to the current pipe
            if (tmp->in_fd == -2)
                tmp->in_fd = mini->pipe_fd_arr[i];
            // If output file descriptor is marked as -3, connect it to the next pipe
            if (tmp->out_fd == -3)
                tmp->out_fd = mini->pipe_fd_arr[i + 3];
            i += 2; // Move to the next pair of pipe file descriptors
        }
        tmp = tmp->next; // Move to the next command
    }
}
