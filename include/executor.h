#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "minishell.h"

void redirect_inputs_outputs(t_minishell *minishell);
void assign_fds(t_minishell *minishell);
void ft_close_two_fd(int *fd1, int *fd2);
void clear_input_on_interrupt(int sig);
void exit_on_interrupt(int sig);

#endif // EXECUTOR_H