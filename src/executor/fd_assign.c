
#include "../include/minishell.h"

// /*
//  * This function is an extension of the assign_fds function and is called when handling the first command in the pipeline (first == TRUE).
//  * 
//  */
// static void assign_fds_ext(t_minishell *minishell, t_cmd *cmd)
// {
//     if (cmd->out_fd == -3)
//     {
//         // Output should be connected to a pipe
//         cmd->out_fd = minishell->pipe_fd_arr[1]; 	// assigns minishell->pipe_fd_arr[1] (write end of the pipe) to cmd->out_fd.
//     }
//     else
//     {
//         // Output is redirected elsewhere, close the write end of the pipe
//         close(minishell->pipe_fd_arr[1]); 
//         minishell->pipe_fd_arr[1] = -2; // Indicate that the pipe is not needed
//     }
// }

// /*
//  * This function assigns file descriptors (in_fd and out_fd) for each command in the pipeline.
//  * It iterates through the linked list of command nodes (t_cmd).
//  * For the first command (first == TRUE), it calls assign_fds_ext.
//  * For subsequent commands (first == FALSE), it assigns file descriptors based on whether input/output redirections are specified. 
//  * It also updates the indices for minishell->pipe_fd_arr.
//  * If a command is the last in the pipeline (cmd->next == NULL), and the input file descriptor is set to -2, 
//  * it means the input should come from a pipe. In this case, it sets cmd->in_fd to minishell->pipe_fd_arr[minishell->nbr_pipes * 2 - 2].
//  * The expression pipe_fd_arr[minishell->nbr_pipes * 2 - 2] is indexing the array pipe_fd_arr to obtain the file descriptor corresponding to the last pipe.
//  * minishell->nbr_pipes: This is the number of pipes needed for the commands. Each pipe requires two file descriptors (one for reading, one for writing).
//  * minishell->nbr_pipes * 2: This calculates the total number of file descriptors needed for all the pipes.
//  * minishell->nbr_pipes * 2 - 2: Subtracting 2 gives the index of the last file descriptor in the array, as array indices are 0-based.
//  * So, pipe_fd_arr[minishell->nbr_pipes * 2 - 2] retrieves the file descriptor for the last pipe's read end. This file descriptor is used to connect the input of the last command to the output of the second-to-last command, effectively chaining the commands together through pipes.
// */
// static void assign_fds(t_minishell *minishell)
// {
//     t_cmd *cmd;
//     int i;

//     i = 0;
//     cmd = minishell->cmds;
//     if (!cmd)
//         return ;
//     assign_fds_ext(minishell, cmd);
//     cmd = cmd->next;
//     while (cmd->next)
//     {
//         // If input file descriptor is unassigned, connect it to the current pipe
//         if (cmd->in_fd == -2)
//             cmd->in_fd = minishell->pipe_fd_arr[i];
//         // If output file descriptor is marked as -3, connect it to the next pipe
//         if (cmd->out_fd == -3)
//             cmd->out_fd = minishell->pipe_fd_arr[i + 3];
//         i += 2; // Move to the next pair of pipe file descriptors
//         cmd = cmd->next; // Move to the next command
//     }
//     // For the last command, if input file descriptor is unassigned, connect it to the last pipe
//     if (cmd->in_fd == -2)
//         cmd->in_fd = minishell->pipe_fd_arr[minishell->nbr_pipes * 2 - 2];
// }


void assign_fds(t_minishell *minishell)
{
    t_cmd *cmd;
    int i;

    if (!cmd)
        return ;
    cmd = minishell->cmd_lst;
    if (cmd->out_fd == -3)
        cmd->out_fd = minishell->pipe_fd_arr[1];
    else
    {
        close(minishell->pipe_fd_arr[1]); 
        minishell->pipe_fd_arr[1] = -2;
    }
    cmd = cmd->next;
    i = 0;
    while (cmd->next)
    {
        if (cmd->in_fd == -2)
            cmd->in_fd = minishell->pipe_fd_arr[i];
        if (cmd->out_fd == -3)
            cmd->out_fd = minishell->pipe_fd_arr[i + 3];
        i += 2;
        cmd = cmd->next;
    }
    if (cmd->in_fd == -2)
        cmd->in_fd = minishell->pipe_fd_arr[minishell->pipe_count * 2 - 2];
}