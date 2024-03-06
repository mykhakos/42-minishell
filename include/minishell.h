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
# include "env.h"
# include "builtins.h"
# include "executor.h"


typedef struct s_minishell
{
    char		**path;      // Array of strings representing the executable search paths
    char		*line;      // Current input line
    int			nbrofcom;   // Number of commands in the input line
    int			nbr_pipes;  // Number of pipes in the input line
    int			*fd_pipes;  // Array of file descriptors for pipes
    char		*prompt;    // Shell prompt
    char		*home;      // Home directory path
    char		*old_cwd;   // Previous working directory path
    int			exitstatus; // Exit status of the last command
    t_env_var	*enviro;    // Linked list representing environment variables
    t_token	    *lex;       // Linked list representing lexical analysis of the input line
    t_cmd   	*cmds;      // Linked list representing parsed commands
}	t_minishell;


t_minishell *new_minishell(void);
void minishell_init(t_minishell *minishell, char **envp);
void minishell_reset(t_minishell *minishell);
void minishell_free(t_minishell **minishell);

#endif