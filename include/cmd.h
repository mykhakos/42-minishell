#ifndef CMD_H
# define CMD_H

#include <stdlib.h>

typedef struct s_cmd
{
    char *exec_path;       // Represents the path to the command executable
    char **cmd_args;       // Represents the array of command arguments
    char *in_redir;        // Represents the type of input redirection (e.g., "<")
    char *in_filename;     // Represents the input file name for redirection
    int in_fd;             // Represents the file descriptor for input redirection
    char *out_redir;       // Represents the type of output redirection (e.g., ">")
    char *out_filename;    // Represents the output file name for redirection
    int out_fd;            // Represents the file descriptor for output redirection
    struct s_cmd *next;    // Pointer to the next node in the linked list
} t_cmd;

t_cmd *new_cmd(void);
void cmd_add_back(t_cmd **cmd_lst, t_cmd *new);
void cmd_free(t_cmd **cmd_lst);


#endif
