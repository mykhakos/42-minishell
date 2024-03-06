#include "include/minishell.h"

/*
 *  Exits the shell with the specified exit code.
 *  If an argument is provided, it is interpreted as the exit code. If no argument is provided, the exit code is 0.
 *  Prints an error message if the argument is not a numeric value.
*/
int	ft_exit(t_minishell *minishell, t_cmd *cmd)
{
	int	res;

	res = 0;
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (cmd->cmd_args[1] && ft_isnum(cmd->cmd_args[1]) == FALSE)
	{
		ft_putstr_fd("exit: ", STDERR_FILENO);
		ft_putstr_fd(cmd->cmd_args[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		res = 2;
	}
	else if (cmd->cmd_args[1] && ft_isnum(cmd->cmd_args[1]) && cmd->cmd_args[2])
	{
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	else if (cmd->cmd_args[1] && ft_isnum(cmd->cmd_args[1]))
		res = ft_atoi(cmd->cmd_args[1]);
	ft_cleaning(minishell);
	exit(res);
}
