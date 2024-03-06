#include "include/minishell.h"

/*
 *
 * implement the behavior of the echo command
*/
int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	no_n;

	i = 0;
	no_n = FALSE;

	// Check if there are no arguments, print a newline, and return
	if (cmd->cmd_args[1] == NULL)
		return (printf("\n"), 0);

	// Check if the first argument is "-n"
	if (ft_strncmp(cmd->cmd_args[1], "-n", ft_strlen("-n") + 1) == 0)
	{
		no_n = TRUE;
		i++;
	}

	// Iterate through the arguments and print them
	while (cmd->cmd_args[++i])
	{
		// Skip empty arguments
		if (cmd->cmd_args[i] && ft_strlen(cmd->cmd_args[i]) == 0)
			continue;

		printf("%s", cmd->cmd_args[i]);

		// Print a space if there is another argument
		if (cmd->cmd_args[i + 1])
			printf(" ");
	}

	// Print a newline unless the "-n" option is specified
	if (no_n == FALSE)
		printf("\n");

	return (0);
}
