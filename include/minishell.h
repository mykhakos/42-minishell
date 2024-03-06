#ifndef MINISHELL_H
# define MINISHELL_H

//#define MAX_LINE 1024
# define TRUE 1
# define FALSE 0


# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <stdarg.h>
// # include <readline/readline.h>
// # include <readline/history.h>
# include <sys/stat.h>
// # include <sys/wait.h>
# include <signal.h>

# include <errno.h>
# include "libft.h"

# include "lexer_parser.h"
# include "cmd.h"
# include "env_var.h"
# include "builtins.h"
# include "executor.h"


typedef struct s_minishell
{
    char		**exec_paths;
    char		*input_line;
    int			cmd_count;
    int			pipe_cound;
    int			*pipe_fd_arr;
    char		*prompt;
    char		*home_dir;
    char		*prev_cwd;
    int			exit_status;
    t_env_var	*env_var_lst;
    t_token	    *token_lst;
    t_cmd   	*cmd_lst;
}	t_minishell;


t_minishell *new_minishell(void);
void minishell_init(t_minishell *minishell, char **envp);
void minishell_reset(t_minishell *minishell);
void minishell_free(t_minishell **minishell);

#endif