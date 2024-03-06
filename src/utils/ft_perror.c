#include "include/minishell.h"

/*
 *
 * ft_perror takes two parameters: msg (the main error message) and utils (additional information or a specific character, possibly a pipe character).
 * It uses the write function to output the main error message and additional information (if provided) to the standard error output.
 * It checks if utils is not NULL before attempting to use it.
 * If utils is not NULL and doesn't represent a pipe character ("|"), it frees the allocated memory for utils 
*/
int	ft_perror(char *msg, char *utils)
{
	write(STDERR_FILENO, msg, ft_strlen(msg));
	if (utils)
	{
		write(STDERR_FILENO, utils, ft_strlen(utils));
		if (ft_strncmp(utils, "|", ft_strlen("|")) != 0)
			free(utils);
	}
	write(STDERR_FILENO, "\n", 1);
	return (EXIT_FAILURE);
}
