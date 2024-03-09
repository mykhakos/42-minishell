#include "include/minishell.h"

/*
 * part executor.exexute_cmd
 * 
*/
int	ft_is_builtin_with_output(char *str)
{
	return (
        ft_strncmp(str, "echo", 5) == 0 ||
        ft_strncmp(str, "pwd", 4) == 0 ||
        ft_strncmp(str, "env", 4) == 0
        );
}

/*
 * part executor
 * Checks if a given command is one of the built-in commands (e.g., echo, pwd, env, cd, export, unset, exit).
*/
int	ft_is_builtin(char *str)
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
 * part export, unset, init
 * extract the "PATH" variable from the environment variables and store its components (directories)
 * in the minishell->path array for later use. 
 * The assumption here is that the "PATH" variable contains directories separated by colons,
 *  and they are stored in minishell->path as an array of strings. 
*/
void ft_update_path(t_minishell *mini)
{
    t_env_var *current;
    current = mini->env_var_lst;

    while (current != NULL)
    {
        if (ft_strncmp(current->key, "PATH", 5) == 0)
        {
            mini->exec_paths = ft_split(current->value, ':');
            return;
        }
        current = current->next;
    }
    mini->exec_paths = NULL;
    return;
}
