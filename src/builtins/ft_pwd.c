#include "include/minishell.h"


/*
 *
 * The function uses the getcwd function to obtain the current working directory and store it in the cwd array.
 * If the getcwd operation is successful (returns a non-NULL value), it prints the current working directory followed by a newline character.
 * The (void)cmd; statement is used to indicate that the cmd parameter is intentionally not used in the function, suppressing any unused parameter warnings from the compiler.
*/
void ft_pwd(t_cmd *cmd)
{
    char cwd[1024];

    // Unused parameter (cmd) to avoid compiler warnings
    (void)cmd;

    // Get the current working directory and print it
    if (getcwd(cwd, 1024) != NULL)
    {
        printf("%s\n", cwd);
    }
}
