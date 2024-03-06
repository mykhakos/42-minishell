#include "../include/minishell.h"


t_minishell *new_minishell(void)
{
    t_minishell *minishell;

    minishell = (t_minishell *)malloc(sizeof(t_minishell));
    if (!minishell)
        return (NULL);
    minishell->line = NULL;
    minishell->cmds = NULL;
    minishell->nbrofcom = 0;
    minishell->nbr_pipes = 0;
    minishell->fd_pipes = NULL;
    minishell->lex = NULL;
    minishell->enviro = NULL;
    minishell->prompt = NULL;
    minishell->exitstatus = 0;
    minishell->old_cwd = NULL;
    minishell->home = NULL;
    minishell->path = NULL;
    return (minishell);
}


/*
 * getenv("USER"): This function is used to retrieve the value of the environment variable with the name "USER." It returns a pointer to the value as a string.
*/
void minishell_init(t_minishell *minishell, char **envp)
{
    char	*tmp;

    // Initialize various fields of the t_minishell structure
    minishell->line = NULL;
    minishell->cmds = NULL;
    minishell->nbrofcom = 0;
    minishell->nbr_pipes = 0;
    minishell->fd_pipes = NULL;
    minishell->lex = NULL;

    // Create a linked list representing environment variables
    minishell->enviro = create_env_list(envp);

    // Set up the shell prompt
    tmp = getenv("USER");
    if (tmp != NULL)
        minishell->prompt = ft_strjoin(tmp, ".minishell ~> ");
    else
        minishell->prompt = ft_strjoin("beautiful", ".minishell ~> ");

    // Initialize exit status and old_cwd
    minishell->exitstatus = 0;
    tmp = getenv("PWD");
    if (tmp != NULL)
        minishell->old_cwd = ft_strdup(tmp);
    else
        minishell->old_cwd = ft_strdup(getenv("HOME"));

    // Initialize the home directory path
    minishell->home = ft_strdup(getenv("HOME"));

    // Fill the path array with executable search paths
    ft_path_filler(minishell);
}


/*
 *
 * The function begins by checking if the minishell->cmds field (presumably representing a list of commands) is not NULL. If it's not NULL, the remove_cmd_list function is called to free the memory associated with the command list.
 * Similarly, the function checks if the minishell->lex field (presumably representing a linked list of lexemes) is not NULL. If it's not NULL, the free_list function is called to free the memory associated with the lexeme list.
 * The function then resets the fields minishell->nbrofcom and minishell->nbr_pipes to zero, indicating no commands and no pipes.
 * The function checks if the minishell->fd_pipes field (presumably representing an array of file descriptors for pipes) is not NULL. If it's not NULL, the free function is called to free the memory associated with the array.
 * The function checks if the minishell->line field (presumably representing an input line) is not NULL. If it's not NULL, the free function is called to free the memory associated with the input line.
 * Finally, the function resets the pointers minishell->fd_pipes, minishell->lex, and minishell->cmds to NULL, indicating that these structures are now empty or uninitialized.
*/
void minishell_reset(t_minishell *minishell)
{
    // Remove the command list
    if (minishell->cmds)
        remove_cmd_list(&minishell->cmds);

    // Free the linked list representing the lexemes
    if (minishell->lex)
        free_list(minishell->lex);

    // Reset the number of commands and number of pipes
    minishell->nbrofcom = 0;
    minishell->nbr_pipes = 0;

    // Free the array storing file descriptors for pipes
    if (minishell->fd_pipes)
        free(minishell->fd_pipes);

    // Free the allocated memory for the input line
    if (minishell->line)
        free(minishell->line);

    // Reset pointers to NULL
    minishell->fd_pipes = NULL;
    minishell->lex = NULL;
    minishell->cmds = NULL;
}


void minishell_free(t_minishell **minishell)
{
    // Free the path array
    path_clean(*minishell);

    // Clear the history of readline
    rl_clear_history();

    // Free the allocated memory for the input line
    if ((*minishell)->line)
        free((*minishell)->line);

    // Free the array storing file descriptors for pipes
    if ((*minishell)->fd_pipes)
        free((*minishell)->fd_pipes);

    // Free the allocated memory for the prompt
    if ((*minishell)->prompt)
        free((*minishell)->prompt);

    // Remove the linked list representing environment variables
    remove_env_list(&(*minishell)->enviro);

    // Remove the command list
    if ((*minishell)->cmds)
        remove_cmd_list(&(*minishell)->cmds);

    // Free the linked list representing the lexemes
    if ((*minishell)->lex)
        free_list((*minishell)->lex);

    // Free the allocated memory for the home directory path
    if ((*minishell)->home)
        free((*minishell)->home);

    // Free the allocated memory for the old working directory path
    if ((*minishell)->old_cwd)
        free((*minishell)->old_cwd);

    // Free the allocated memory for the minishell structure
    free(*minishell);
    *minishell = NULL;
}