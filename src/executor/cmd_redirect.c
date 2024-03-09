#include "../include/minishell.h"


/*
 * part open redirs
 * If the redir_in field of the command is not NULL and its value is "<<", it duplicates the standard input file descriptor 
 *(STDIN_FILENO) using the dup system call. 
 * This is often used for here documents or here strings, where the input is provided directly in the command.
 * If the redir_in field is not NULL and does not match "<<", it assumes that the input is redirected from a file. 
 * In this case, it opens the specified file (node->in_name) in read-only mode using the open system call.
*/
static void open_infile(t_cmd *node, int *exit_st)
{
    *exit_st = 0;
    if (node->redir_in && ft_strncmp(node->redir_in, "<<", ft_strlen("<<")) == 0)
    {
        // If input redirection is "<<", duplicate STDIN_FILENO
        node->in_fd = dup(STDIN_FILENO);  // from unistd.h
        if (node->in_fd == -1)
        {
            // Handle error if duplication fails
            ft_strerror(strerror(errno), node->in_name);
            *exit_st = 1; // Set exit status flag to 1
        }
    }
    else if (node->redir_in)
    {
        // If input redirection is a file name, open the file in read-only mode
        node->in_fd = open(node->in_name, O_RDONLY, 0666);
        if (node->in_fd == -1)
        {
            // Handle error if file opening fails
            ft_strerror(strerror(errno), node->in_name);
            *exit_st = 1; // Set exit status flag to 1
        }
    }
}

/*
 * part open redirs
 * function is responsible for handling the output file redirection for a given command (t_cmd).
 * It checks the type of output redirection specified in the command (node->redir_out) and performs the necessary actions 
 * to set up the output file descriptor (node->out_fd).
 * If the redir_out field of the command is not NULL, it checks whether the value is ">>" (append mode) using ft_strncmp. If true, it opens the specified file (node->out_name) in append mode (O_APPEND). Otherwise, it opens the file in write mode with truncation (O_TRUNC).
 * If the output file descriptor (node->out_fd) is -1 after attempting to open the file, it indicates that an error occurred. In this case, it sets the exit_st flag to 1.
 * If the file opening is successful, it sets the exit_st flag to 0.
 *
*/
static void open_outfile(t_cmd *node, int *exit_st)
{
    if (node->redir_out)
    {
        if (node->redir_out && ft_strncmp(node->redir_out, ">>", ft_strlen(">>") + 1) == 0)
            node->out_fd = open(node->out_name, O_CREAT | O_RDWR | O_APPEND, 0666);
        else
            node->out_fd = open(node->out_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
    }

    if (node->out_fd == -1)
    {
        // Handle error if file opening fails
        ft_strerror(strerror(errno), node->in_name);
        *exit_st = 1; // Set exit status flag to 1
    }
    else
    {
        *exit_st = 0; // Set exit status flag to 0 if file opening is successful
    }
}

/*
 * part executor
 * The ft_redirect_inputs_outputs function is responsible for iterating over the commands in the mini data structure 
 * and performing the necessary setup for input and output redirection for each command by calling
 * the open_outfile and open_infile functions.
*/
void ft_redirect_inputs_outputs(t_minishell *mini)
{
    t_cmd *tmp;

    // Iterate over the commands in the mini data structure
    tmp = mini->cmd_lst;
    while (tmp)
    {
        // Handle output file redirection
        open_outfile(tmp, &mini->exit_status);

        // Handle input file redirection
        open_infile(tmp, &mini->exit_status);

        // Move to the next command
        tmp = tmp->next;
    }
}