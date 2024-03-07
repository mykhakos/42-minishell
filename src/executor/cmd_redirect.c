#include "../include/minishell.h"


/*
* function is responsible for handling the input file redirection for a given command (t_cmd).
* It checks the type of input redirection specified in the command (node->in_redir) and performs the necessary actions 
* to set up the input file descriptor (node->in_fd).
* If the in_redir field of the command is not NULL, it checks whether the value is "<<" using ft_strncmp. If true, it duplicates the standard input file descriptor (STDIN_FILENO) using the dup function. Otherwise, it opens the file specified in the in_filename field of the command in read mode (O_RDONLY).
* If the input file descriptor (node->in_fd) is -1 after attempting to open the file, it indicates that an error occurred. In this case, it sets the exit_status flag to 1.
* If the file opening is successful, it sets the exit_status flag to 0.
*
*/
static void redirect_input(t_cmd *node, int *exit_status)
{
    if (node->in_redir)
    {
        if (ft_strncmp(node->in_redir, "<<", ft_strlen("<<")) == 0)
        {
            node->in_fd = dup(STDIN_FILENO);
        }
        else
        {
            node->in_fd = open(node->in_filename, O_RDONLY, 0666);
            if (node->in_fd == -1)
            {
                ft_strerror(strerror(errno), node->in_filename);
                *exit_status = 1;
            }
        }
    }
    else
    {
        *exit_status = 0;
    }
}

/*
* function is responsible for handling the output file redirection for a given command (t_cmd).
* It checks the type of output redirection specified in the command (node->out_redir) and performs the necessary actions
* to set up the output file descriptor (node->out_fd).
* If the out_redir field of the command is not NULL, it checks whether the value is ">>" using ft_strncmp. If true, it opens the file specified in the out_filename field of the command in read/write mode (O_CREAT | O_RDWR | O_APPEND). Otherwise, it opens the file specified in the out_filename field of the command in read/write mode (O_CREAT | O_RDWR | O_TRUNC).
* If the output file descriptor (node->out_fd) is -1 after attempting to open the file, it indicates that an error occurred. In this case, it sets the exit_status flag to 1.
* If the file opening is successful, it sets the exit_status flag to 0.
*
*/
static void redirect_output(t_cmd *node, int *exit_status)
{
    if (node->out_redir)
    {
        if (ft_strncmp(node->out_redir, ">>", ft_strlen(">>") + 1) == 0)
        {
            node->out_fd = open(node->out_filename, O_CREAT | O_RDWR | O_APPEND, 0666);
        }
        else
        {
            node->out_fd = open(node->out_filename, O_CREAT | O_RDWR | O_TRUNC, 0666);
            if (node->out_fd == -1)
            {
                ft_strerror(strerror(errno), node->out_filename);
                *exit_status = 1;
            }
        }
    }
    else
    {
        *exit_status = 0;
    }
}

/*
* function is responsible for redirecting the input and output file descriptors for each command in the command list (t_cmd).
* It iterates through the command list and calls the redirect_input and redirect_output functions for each command.
*
*/
void redirect_inputs_outputs(t_minishell *minishell)
{
    t_cmd *cmd;

    cmd = minishell->cmd_lst;
    while (cmd)
    {
        redirect_input(cmd, &minishell->exit_status);
        redirect_output(cmd, &minishell->exit_status);
        cmd = cmd->next;
    }
}
