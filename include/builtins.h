#ifndef BUILTINS_H
#define BUILTINS_H

#include "minishell.h"

int	ft_cd(t_minishell *minishell, t_cmd *cmd);
int    ft_echo(t_cmd *cmd);
int    ft_env(t_minishell *minishell);
int    ft_exit(t_minishell *minishell, t_cmd *cmd);
int    ft_export(t_minishell *minishell, t_cmd *cmd);
void    ft_pwd(t_cmd *cmd);
int    ft_unset(t_minishell *minishell, t_cmd *cmd);

void update_path(t_minishell *minishell);

#endif // BUILTINS_H