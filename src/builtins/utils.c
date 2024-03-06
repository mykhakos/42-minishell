#include "include/minishell.h"


int	is_builtin_with_output(char *str)
{
	return (
        ft_strncmp(str, "echo", 5) == 0 ||
        ft_strncmp(str, "pwd", 4) == 0 ||
        ft_strncmp(str, "env", 4) == 0
        );
}

/*
 * Checks if a given command is one of the built-in commands (e.g., echo, pwd, env, cd, export, unset, exit).
*/
int	is_builtin(char *str)
{
	return (
        is_builtin_with_output(str) ||
        ft_strncmp(str, "cd", 3) == 0 ||
        ft_strncmp(str, "export", 7) == 0 ||
        ft_strncmp(str, "unset", 6) == 0 ||
        ft_strncmp(str, "exit", 5) == 0
        );
}

/*
 * extract the "PATH" variable from the environment variables and store its components (directories)
 * in the minishell->path array for later use. 
 * The assumption here is that the "PATH" variable contains directories separated by colons,
 *  and they are stored in minishell->path as an array of strings. 
*/
void update_path(t_minishell *minishell)
{
    t_node *curr;

    curr = minishell->enviro;
    while (curr != NULL)
    {
        if (ft_strncmp(curr->key, "PATH", 5) == 0)
        {
            minishell->path = ft_split(curr->value, ':');
            return ;
        }
        curr = curr->next;
    }
    minishell->path = NULL;
    return;
}