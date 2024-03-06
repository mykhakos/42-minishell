
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <stdarg.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <signal.h>

# include <errno.h>
# include "libft.h"
//# include "../libft/libft.h"

//#define MAX_LINE 1024
# define TRUE 1
# define FALSE 0

/*
 * defines an enumeration named e_token with various token types used in a lexical analysis context. 
*/
enum e_token
{
    WORD = -1,        // Represents a word token
    WHITE_SPACE = ' ', // Represents a white space token
    QOUTE = '\'',      // Represents a single quote token
    DOUBLE_QUOTE = '\"', // Represents a double quote token
    ENV = '$',         // Represents an environment variable token
    PIPE_LINE = '|',   // Represents a pipe line token
    REDIR_IN = '<',    // Represents a input redirection token
    REDIR_OUT = '>',   // Represents an output redirection token
    HERE_DOC,         // Represents a here document token, often used for input redirection.
    DREDIR_OUT,       // epresents a double output redirection token (>>), used to append output to a file.
};

typedef struct s_elem		t_elem;
typedef struct s_redir_elem	t_redir_elem;
typedef union u_union		t_union;
typedef struct s_pipe		t_pipe;
typedef struct s_redir_list	t_redir_list;

/*
 * This enumeration represents different states that the lexer can be in. 
 * IN_DQUOTE: Inside double quotes.
 * IN_QUOTE: Inside single quotes.
 * GENERAL: General state, outside of quotes.
 */
enum e_state
{
	IN_DQUOTE,
	IN_QUOTE,
	GENERAL,
};

/*
 * Represents an element (token) in the linked list.
 */
typedef struct s_elem
{
    char *content;          // Represents the content or value of the token
    int len;                // Represents the length of the content
    enum e_token type;      // Represents the type of the token (from the e_token enumeration)
    enum e_state state;     // Represents the state of the lexer when processing this token
    struct s_elem *next;    // Pointer to the next token in the linked list
    struct s_elem *prev;    // Pointer to the previous token in the linked list
} t_elem;

/*
 * Represents a linked list of tokens for lexer
*/
typedef struct s_list_n
{
    t_elem *head;   // Pointer to the first token (head) in the linked list
    t_elem *tail;   // Pointer to the last token (tail) in the linked list
    int size;       // Represents the number of tokens in the linked list
} t_list_n;

typedef struct s_node_cmd
{
    char *path;                 // Represents the path to the command executable
    char **cmd;                 // Represents the array of command arguments
    char *redir_in;             // Represents the type of input redirection (e.g., "<")
    char *in_name;              // Represents the input file name for redirection
    int in_fd;                  // Represents the file descriptor for input redirection
    char *redir_out;            // Represents the type of output redirection (e.g., ">")
    char *out_name;             // Represents the output file name for redirection
    int out_fd;                 // Represents the file descriptor for output redirection
    struct s_node_cmd *next;    // Pointer to the next node in the linked list
} t_node_cmd;

/*
linked list for environment variables
*/
typedef struct s_node
{
    char *key;           // Represents the key of the environment variable
    char eq;             // Represents the equals sign ('=')
    char *value;         // Represents the value of the environment variable
    struct s_node *next; // Pointer to the next node in the linked list
} t_node;

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
    t_node		*enviro;    // Linked list representing environment variables
    t_list_n	*lex;       // Linked list representing lexical analysis of the input line
    t_node_cmd	*cmds;      // Linked list representing parsed commands
}	t_minishell;

void	path_clean(t_minishell *minishell)
{
	int	i;

	i = -1;
	if (minishell->path)
	{
		while (minishell->path[++i])
			free(minishell->path[i]);
		free(minishell->path);
	}
}

/*
 *  function is responsible for deallocating the memory occupied by a linked list of environment variables (t_node structure)

void	remove_env_list(t_node **head)
{
	t_node	*tmp;
	t_node	*current;

	if (head == NULL || *head == NULL)
		return ;
	current = *head;
	while (current != NULL)
	{
		tmp = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = tmp;
	}
}
*/

/*
static void	remove_cmd_list_ext(t_node_cmd *current)
{
	int	i;

	i = 0;
	if (current->path)
		free(current->path);
	if (current->redir_in)
		free(current->redir_in);
	if (current->in_name)
		free(current->in_name);
	if (current->redir_out)
		free(current->redir_out);
	if (current->out_name)
		free(current->out_name);
	if (current->cmd)
	{
		while (current->cmd[i])
			free(current->cmd[i++]);
		free(current->cmd);
	}
	if (current->in_fd > 0)
		close(current->in_fd);
	if (current->out_fd > 0)
		close(current->out_fd);
}
*/

/*
 * A loop is used to traverse the list:
 * The tmp pointer is used to temporarily store the address of the next node.
 * The remove_cmd_list_ext function is called to free memory for the current node.
 * Memory for the current node is freed using free.
 * The current pointer is updated to point to the next node (current = tmp).
 * After the loop, the head pointer (*head) is set to NULL to indicate an empty list.

void	remove_cmd_list(t_node_cmd **head)
{
	t_node_cmd	*tmp;
	t_node_cmd	*current;

	if (head == NULL || *head == NULL)
		return ;
	current = *head;
	while (current != NULL)
	{
		tmp = current->next;
		remove_cmd_list_ext(current);
		free(current);
		current = tmp;
	}
	*head = NULL;
}
 */

/*
 *
 *loop is used to traverse the list:
 * The next pointer is used to temporarily store the address of the next element.
 * Memory for the content of the current element (current->content) is freed using free.
 * Memory for the current element (current) is freed using free.
 * The current pointer is updated to point to the next element (current = next).
 * After the loop, memory for the list structure (list) is freed using free.

void	free_list(t_list_n *list)
{
	t_elem	*next;
	t_elem	*current;

	if (list == NULL)
		return ;
	current = list->head;
	while (current)
	{
		next = current->next;
		free(current->content);
		free(current);
		current = next;
	}
	free(list);
}
 */

/*
void	ft_cleaning(t_minishell *minishell)  // moved to main/minishell.c/minishell_free
{
	path_clean(minishell);
	rl_clear_history();
	if (minishell->line)
		free(minishell->line);
	if (minishell->fd_pipes)
		free(minishell->fd_pipes);
	free(minishell->prompt);
	remove_env_list(&minishell->enviro);
	if (minishell->cmds)
		remove_cmd_list(&minishell->cmds);
	if (minishell->lex)
		free_list(minishell->lex);
	if (minishell->home)
		free(minishell->home);
	if (minishell->old_cwd)
		free(minishell->old_cwd);
}
 */

/*
 * function effectively 
 * extracts the key part of an environment variable string by copying characters until the equals sign.
 * It then returns a dynamically allocated string containing the key.

char *ft_separate_key(char *env)
{
    int i;
    char *key;

    // Check if the input is NULL
    if (!env)
        return (NULL);

    // Find the index of the equals sign ('=')
    i = 0;
    while (env[i] != '=')
        i++;

    // Allocate memory for the key
    key = ft_calloc(sizeof(char), (i + 1));
    if (key == NULL)
        return (NULL);

    // Copy the characters until the equals sign to the key
    i = 0;
    while (env[i] != '=')
    {
        key[i] = env[i];
        i++;
    }

    // Null-terminate the key
    key[i] = '\0';

    return (key);
}

*/

/*
 * function effectively extracts the value part of an environment variable string 
 * by copying characters from the position following the equals sign until the end of the string. 
 * 

char *ft_separate_value(char *env)
{
    int i;
    int y;
    char *value;

    // Check if the input is NULL
    if (!env)
        return (NULL);

    // Find the index of the equals sign ('=')
    i = 0;
    while (env[i] != '=')
        i++;

    // Increment i to move to the character following the equals sign
    i++;

    // Find the total length of the remaining string
    y = i;
    while (env[y])
        y++;

    // Allocate memory for the value
    value = ft_calloc(sizeof(char), ((y - i) + 1));
    if (!value)
        return (NULL);

    // Copy the characters from the input string to the value
    y = 0;
    while (env[i])
    {
        value[y] = env[i];
        i++;
        y++;
    }

    // Null-terminate the value
    value[y] = '\0';

    return (value);
}
*/

/*
 *   Checks if an environment variable specified in cmd->cmd[1] exists in the linked list.
 *   If it exists, replaces the current value with the new one obtained from cmd->cmd[1].
 *   Returns TRUE if the replacement is successful, otherwise returns FALSE.

int	check_env_var_and_repl(t_node **head, t_node_cmd *cmd)  // moved: builtins/ft_export.c/update_env_var
{
	t_node	*cur;
	char	*tmp;

	cur = *head;
	tmp = ft_separate_key(cmd->cmd[1]);
	while (cur)
	{
		if (ft_strncmp(cur->key, tmp, ft_strlen(tmp) + 1) == 0)
		{
			free(cur->value);
			cur->value = ft_separate_value(cmd->cmd[1]);
			free(tmp);
			return (TRUE);
		}
		cur = cur->next;
	}
	free(tmp);
	return (FALSE);
}
*/

/*
 * Updates the values of environment variables "PWD" and "OLDPWD" 
 * based on the current working directory (cwd) and previous working directory (pcwd).

void	ft_update_cwd(t_minishell *minishell, char *cwd, char *pcwd)  // moved: builtins/ft_cd.c
{
	t_node	*current;

	current = minishell->enviro;
	while (current != NULL)
	{
		if (ft_strncmp(current->key, "PWD", 4) == 0)
		{
			free(current->value);
			current->value = ft_strdup(cwd);
		}
		if (ft_strncmp(current->key, "OLDPWD", 7) == 0)
		{
			free(current->value);
			current->value = ft_strdup(pcwd);
		}
		current = current->next;
	}
}
*/

/*
 * Processes the command argument for cd and returns the updated path. 
 * It handles relative paths such as ".", "..", "./", and "../" by modifying the current working directory accordingly.

char *dot_ext(char *pcwd, t_node_cmd *cmd)
{
	char *path[2];
	char *tmp;

	// Initialize variables
	path[0] = NULL;
	path[1] = NULL;
	tmp = pcwd;

	// Check if the command argument is ".", "./", or "../"
	if (!ft_strncmp(cmd->cmd[1], ".", 2) || !ft_strncmp(cmd->cmd[1], "./", 3))
		return (ft_strdup(pcwd));
	else if (ft_strncmp(cmd->cmd[1], "..", 3) == 0 || ft_strncmp(cmd->cmd[1], "../", 4) == 0)
		return (ft_substr(pcwd, 0, ft_strrchr(pcwd, '/') - pcwd));

	// Handle cases where the command argument starts with "../"
	path[0] = ft_strdup(pcwd);
	while (ft_strncmp(cmd->cmd[1], "../", 3) == 0)
	{
		// Update path to parent directory
		path[1] = ft_substr(path[0], 0, ft_strrchr(path[0], '/') - path[0]);
		free(path[0]);
		path[0] = path[1];

		// Update command argument to remove "../"
		tmp = ft_substr(cmd->cmd[1], 3, ft_strlen(cmd->cmd[1]) - 3);
		free(cmd->cmd[1]);
		cmd->cmd[1] = tmp;
	}

	// Combine path components
	path[1] = ft_strjoin(path[0], "/");
	free(path[0]);
	tmp = ft_strjoin(path[1], cmd->cmd[1]);

	// Clean up and return the updated path
	return (free(path[1]), tmp);
}

*/

/*
 *  Handles special cases for constructing the path argument for the cd command.
 *  Calls dot_ext when the argument starts with a dot ('.').
 *  Combines the home directory with the specified path in cmd->cmd[1] and returns the result.

char	*get_arg_cd_ext(char *home, char *pcwd, t_node_cmd *cmd, char c)
{
	char	*res;
	char	*tmp;

	tmp = NULL;
	res = NULL;
	if (c == '.')
		res = dot_ext(pcwd, cmd);
	else
	{
		if (ft_strlen(cmd->cmd[1]) == 1)
			return (ft_strdup(home));
		if (cmd->cmd[1][1] == '/')
			res = ft_strjoin(home, cmd->cmd[1] + 1);
		else
		{
			tmp = ft_strjoin(home, "/");
			res = ft_strjoin(tmp, cmd->cmd[1]);
			free(tmp);
		}
	}
	return (res);
}
*/

/*
 *  Constructs the path argument for the cd command based on different cases.
 *  If no argument is provided, returns the home directory.
 *  If the argument is "-", returns the previous working directory.
 *  Handles cases where the argument starts with "~" or ".", calling get_arg_cd_ext accordingly.
 *  Otherwise, returns the specified argument.

char	*get_arg_cd(t_node_cmd *cmd, char *home, char *old_cwd, char *pcwd) // moved to builtins/ft_cd.c
{
	char	*res;

	res = NULL;
	if (cmd->cmd[1] == NULL)
	{
		if (home == NULL)
			return (res);
		res = ft_strdup(home);
	}
	else if (cmd->cmd[1] && ft_strncmp(cmd->cmd[1], "-", 2) == 0)
		return (ft_strdup(old_cwd));
	else if (cmd->cmd[1] && (cmd->cmd[1][0] == '~' || cmd->cmd[1][0] == '.'))
		res = get_arg_cd_ext(home, pcwd, cmd, cmd->cmd[1][0]);
	else
		res = ft_strdup(cmd->cmd[1]);
	return (res);
}
*/

/*
 * Checks if a given string represents a numeric value.

int	ft_isnum(char *arg) // moved: utils/ft_isnum.c
{
	int	i;

	i = 0;
	while (arg[i] != '\0')
	{
		if (arg[i] < '0' || arg[i] > '9')
			return (FALSE);
		i++;
	}
	return (TRUE);
}
*/

/*
static void ft_remove_node_ext(t_node *to_remove)
{
	// Free memory for the key, value, and the node itself
	free(to_remove->key);
	free(to_remove->value);
	free(to_remove);
}

void ft_remove_node(t_node **head, char *key)
{
	t_node *current;
	t_node *prev;
	t_node *to_remove;
	int first;

	// Check if the head of the list is NULL or if the key is NULL
	if (*head == NULL || !key)
		return;

	// Initialize pointers
	current = *head;
	first = TRUE;

	// Traverse the list
	while (current)
	{
		// Check if the key matches
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0)
		{
			to_remove = current;

			// Update the head if the first element is being removed
			if (first)
				*head = current->next;
            else
                prev->next = current->next;

            // Remove the node
            ft_remove_node_ext(to_remove);
            return;
        }

        // Update pointers for the next iteration
        first = FALSE;
        prev = current;
        current = current->next;
    }
}
*/

/*
 * extract the "PATH" variable from the environment variables and store its components (directories)
 * in the minishell->path array for later use. 
 * The assumption here is that the "PATH" variable contains directories separated by colons,
 *  and they are stored in minishell->path as an array of strings. 

void ft_path_filler(t_minishell *minishell) // moved to builtins/utils.c/update_path
{
    t_node *current;

    // Initialize the current node to the head of the linked list
    current = minishell->enviro;

    // Traverse the linked list
    while (current != NULL)
    {
        // Check if the current node's key is "PATH"
        if (ft_strncmp(current->key, "PATH", 5) == 0)
        {
            // Split the value of "PATH" using ':' as the delimiter and store in minishell->path
            minishell->path = ft_split(current->value, ':');
            return;
        }

        // Move to the next node in the linked list
        current = current->next;
    }

    // If "PATH" is not found, set minishell->path to NULL
    minishell->path = NULL;
    return;
}

*/

/*
void	ft_strerror(char *msg, char *name)
{
	ft_putstr_fd(msg, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(name, 2);
	ft_putstr_fd("\n", 2);
}
 */

/*
 *  Checks if a command is a valid identifier for the export command.
 *  Verifies that the command starts with an alphabet character or an underscore, and contains an equal sign (=).

static int	ex_command_tester(char *comm)  // moved: builtins/ft_export.c/ex_command_tester
{
	int	i;

	i = 0;
	if (ft_strlen(comm) <= 1)
	{
		ft_strerror(comm, "not a valid identifier");
		return (0);
	}
	if (!ft_isalpha(comm[0]) && comm[0] != 95)
	{
		ft_strerror(comm, "not a valid identifier");
		return (0);
	}
	while (comm[i])
	{
		if (comm[i] == 61)
			return (1);
		i++;
	}
	ft_strerror(comm, "not a valid identifier");
	return (0);
}
*/


/*
 * linked list manipulation, specifically adding nodes to the end of the linked list to represent environment variables.
 * It extracts the key and value from the environment variable string env using the ft_separate_key and ft_separate_value functions.

void ft_add_end(t_node **head, char *env)
{
    t_node *new_node;
    t_node *current;

    // Allocate memory for a new node
    new_node = ft_calloc(sizeof(t_node), 1);
    if (new_node == NULL)
        return;

    // Set the key, equals sign, and value of the new node based on the environment variable
    new_node->key = ft_separate_key(env);
    new_node->eq = '=';
    new_node->value = ft_separate_value(env);
    new_node->next = NULL;

    // If the linked list is empty, make the new node the head
    if (*head == NULL)
    {
        *head = new_node;
        return;
    }

    // Traverse the linked list to find the last node
    current = *head;
    while (current->next != NULL)
        current = current->next;

    // Add the new node to the end of the linked list
    current->next = new_node;
}
 */

/*  ********  BUILTINS ********* */

/*
int	ft_echo(t_node_cmd *cmd)
{
	int	i;
	int	no_n;

	i = 0;
	no_n = FALSE;

	// Check if there are no arguments, print a newline, and return
	if (cmd->cmd[1] == NULL)
		return (printf("\n"), 0);

	// Check if the first argument is "-n"
	if (ft_strncmp(cmd->cmd[1], "-n", ft_strlen("-n") + 1) == 0)
	{
		no_n = TRUE;
		i++;
	}

	// Iterate through the arguments and print them
	while (cmd->cmd[++i])
	{
		// Skip empty arguments
		if (cmd->cmd[i] && ft_strlen(cmd->cmd[i]) == 0)
			continue;

		printf("%s", cmd->cmd[i]);

		// Print a space if there is another argument
		if (cmd->cmd[i + 1])
			printf(" ");
	}

	// Print a newline unless the "-n" option is specified
	if (no_n == FALSE)
		printf("\n");

	return (0);
}

void ft_pwd(t_node_cmd *cmd)
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

void ft_env(t_minishell *minishell)
{
    t_node *current;

    // Check if the environment linked list is not empty
    if (minishell->enviro)
    {
        // Iterate through the linked list and print environment variables
        current = minishell->enviro;
        while (current != NULL)
        {
            // Skip printing specific environment variables (LINES, COLUMNS)
            if (ft_strncmp(current->key, "LINES", 6) == 0
                || ft_strncmp(current->key, "COLUMNS", 8) == 0)
            {
                current = current->next;
                continue;
            }

            // Print the key, equal sign, and value of the current environment variable
            printf("%s%c%s\n", current->key, current->eq, current->value);

            // Move to the next node in the linked list
            current = current->next;
        }
    }
}

int	ft_cd(t_minishell *minishell, t_node_cmd *cmd)
{
	char	cwd[1024];
	char	pcwd[1024];
	char	*dest;

	if (cmd->cmd[1] && cmd->cmd[2] != NULL)
	{
		ft_strerror("too many arguments", cmd->cmd[0]);
		return (1);
	}
	getcwd(pcwd, 1024);
	dest = get_arg_cd(cmd, minishell->home, minishell->old_cwd, pcwd);
	if (chdir(dest) == -1)
	{
		ft_strerror(strerror(errno), dest);
		free(dest);
		return (1);
	}
	getcwd(cwd, 1024);
	free(minishell->old_cwd);
	minishell->old_cwd = ft_strdup(pcwd);
	ft_update_cwd(mini, cwd, pcwd);
	free(dest);
	return (0);
}

int	ft_export(t_minishell *minishell, t_node_cmd *cmd)
{
	t_node	*cur;

	if (cmd->cmd[1])
	{
		if (!ex_command_tester(cmd->cmd[1]))
			return (2);
		if (check_env_var_and_repl(&minishell->enviro, cmd) == FALSE)
			ft_add_end(&minishell->enviro, cmd->cmd[1]);
		path_clean(minishell);
		ft_path_filler(mini);
		return (0);
	}
	cur = minishell->enviro;
	while (cur != NULL)
	{
		if (ft_strncmp(cur->key, "LINES", 6) != 0
			|| ft_strncmp(cur->key, "COLUMNS", 8) != 0)
			printf("declare -x %s%c\"%s\"\n", cur->key, cur->eq, cur->value);
		cur = cur->next;
	}
	return (0);
}

int	ft_unset(t_minishell *minishell, t_node_cmd *cmd)
{
	t_node	*current;
	int		i;

	i = 0;
	if (!cmd->cmd[1])
		return (0);
	current = minishell->enviro;
	while (current != NULL)
	{
		if (ft_strncmp(current->key, cmd->cmd[1], ft_strlen(cmd->cmd[1])) == 0)
		{
			ft_remove_node(&minishell->enviro, cmd->cmd[1]);
			if (minishell->path != NULL)
			{
				while (minishell->path[i])
					free(minishell->path[i++]);
				free(minishell->path);
				minishell->path = NULL;
			}
			return (ft_path_filler(mini), 0);
		}
		else
			current = current->next;
	}
	return (0);
}

int	ft_exit(t_minishell *minishell, t_node_cmd *cmd)
{
	int	res;

	res = 0;
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (cmd->cmd[1] && ft_isnum(cmd->cmd[1]) == FALSE)
	{
		ft_putstr_fd("exit: ", STDERR_FILENO);
		ft_putstr_fd(cmd->cmd[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		res = 2;
	}
	else if (cmd->cmd[1] && ft_isnum(cmd->cmd[1]) && cmd->cmd[2])
	{
		ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	else if (cmd->cmd[1] && ft_isnum(cmd->cmd[1]))
		res = ft_atoi(cmd->cmd[1]);
	ft_cleaning(mini);
	exit(res);
}
*/

/*
int	ft_is_builtin(char *str)
{
	if (ft_strncmp(str, "echo", 5) == 0)
		return (TRUE);
	else if (ft_strncmp(str, "pwd", 4) == 0)
		return (TRUE);
	else if (ft_strncmp(str, "env", 4) == 0)
		return (TRUE);
	else if (ft_strncmp(str, "cd", 3) == 0)
		return (TRUE);
	else if (ft_strncmp(str, "export", 7) == 0)
		return (TRUE);
	else if (ft_strncmp(str, "unset", 6) == 0)
		return (TRUE);
	else if (ft_strncmp(str, "exit", 5) == 0)
		return (TRUE);
	return (FALSE);
}
*/

/*
 * Executes a built-in command by calling the appropriate function
 * for the corresponding built-in command (e.g., ft_cd, ft_export, ft_unset, ft_exit).
 * handles functions that do not have output to console.

int	ft_execute_builtin(t_minishell *minishell, t_node_cmd *cmd) // moved: executor/executor.c/execute_builtin
{
	int	res;

	res = 0;
	if (cmd->in_fd > 0)
		close(cmd->in_fd);
	if (cmd->out_fd > 0)
		close(cmd->out_fd);
	if (ft_strncmp(cmd->cmd[0], "cd", 3) == 0)
		res = ft_cd(minishell, cmd);
	else if (ft_strncmp(cmd->cmd[0], "export", 7) == 0)
		res = ft_export(minishell, cmd);
	else if (ft_strncmp(cmd->cmd[0], "unset", 6) == 0)
		res = ft_unset(minishell, cmd);
	else if (ft_strncmp(cmd->cmd[0], "exit", 5) == 0)
		res = ft_exit(minishell, cmd);
	return (res);
}
*/



/*
 * function is responsible for creating pipes needed for inter-process communication between commands.
 * It allocates an array of file descriptors (minishell->fd_pipes) and initializes them using the pipe system call.
 * checks if there is more than one command (minishell->nbrofcom > 1) and dynamically allocates space for the array of file descriptors.
 *  It then uses a loop to create pipes using the pipe system call, updating the array of file descriptors accordingly.
 *

int	create_pipes(t_minishell	*minishell) // moved to executor/executor.c/create_pipes
{
	int	i;

	i = 0;
	if (minishell->nbrofcom > 1)
		minishell->fd_pipes = (int *)malloc(sizeof(int) * minishell->nbr_pipes * 2);
	while (i < minishell->nbr_pipes * 2)
	{
		if (pipe(minishell->fd_pipes + i) == -1) // from unistd.h
			return (ft_strerror("Pipes creating", "error"), -1);
		i += 2;
	}
	return (0);
}

*/



////////////////////////PIPES///////////////////////////

/*
 * If the redir_in field of the command is not NULL and its value is "<<", it duplicates the standard input file descriptor 
 *(STDIN_FILENO) using the dup system call. 
 * This is often used for here documents or here strings, where the input is provided directly in the command.
 * If the redir_in field is not NULL and does not match "<<", it assumes that the input is redirected from a file. 
 * In this case, it opens the specified file (node->in_name) in read-only mode using the open system call.

static void open_infile(t_node_cmd *node, int *exit_st)  // moved: executor/cmd_redirect.c/redirect_input
{
    *exit_st = 0;
    if (node->redir_in && ft_strncmp(node->redir_in, "<<", ft_strlen("<<")) == 0)
    {
        // If input redirection is "<<", duplicate STDIN_FILENO
        node->in_fd = dup(STDIN_FILENO);  // from unistd.h
        if (node->in_fd == -1)
        {
            // Handle error if duplication fails
            ft_strerror(strerror(errno), node->in_name);
            *exit_st = 1; // Set exit status flag to 1
        }
    }
    else if (node->redir_in)
    {
        // If input redirection is a file name, open the file in read-only mode
        node->in_fd = open(node->in_name, O_RDONLY, 0666);
        if (node->in_fd == -1)
        {
            // Handle error if file opening fails
            ft_strerror(strerror(errno), node->in_name);
            *exit_st = 1; // Set exit status flag to 1
        }
    }
}
*/

/*
 * function is responsible for handling the output file redirection for a given command (t_node_cmd).
 * It checks the type of output redirection specified in the command (node->redir_out) and performs the necessary actions 
 * to set up the output file descriptor (node->out_fd).
 * If the redir_out field of the command is not NULL, it checks whether the value is ">>" (append mode) using ft_strncmp. If true, it opens the specified file (node->out_name) in append mode (O_APPEND). Otherwise, it opens the file in write mode with truncation (O_TRUNC).
 * If the output file descriptor (node->out_fd) is -1 after attempting to open the file, it indicates that an error occurred. In this case, it sets the exit_st flag to 1.
 * If the file opening is successful, it sets the exit_st flag to 0.
 *

static void open_outfile(t_node_cmd *node, int *exit_st)  // moved: executor/cmd_redirect.c/redirect_output
{
    if (node->redir_out)
    {
        if (node->redir_out && ft_strncmp(node->redir_out, ">>", ft_strlen(">>") + 1) == 0)
            node->out_fd = open(node->out_name, O_CREAT | O_RDWR | O_APPEND, 0666);
        else
            node->out_fd = open(node->out_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
    }

    if (node->out_fd == -1)
    {
        // Handle error if file opening fails
        ft_strerror(strerror(errno), node->in_name);
        *exit_st = 1; // Set exit status flag to 1
    }
    else
    {
        *exit_st = 0; // Set exit status flag to 0 if file opening is successful
    }
}
*/

/*
 * The open_redirs function is responsible for iterating over the commands in the mini data structure 
 * and performing the necessary setup for input and output redirection for each command by calling
 * the open_outfile and open_infile functions.

void open_redirs(t_minishell *minishell)  // moved: executor/cmd_redirect.c/redirect_inputs_outputs
{
    t_node_cmd *tmp;

    // Iterate over the commands in the mini data structure
    tmp = minishell->cmds;
    while (tmp)
    {
        // Handle output file redirection
        open_outfile(tmp, &minishell->exitstatus);

        // Handle input file redirection
        open_infile(tmp, &minishell->exitstatus);

        // Move to the next command
        tmp = tmp->next;
    }
}
*/

////////////////////////PIPES END///////////////////////////



////////////////////////OPEN FDs (BEFORE PIPES)///////////////////////////


/*
 * This function is an extension of the assign_fds function and is called when handling the first command in the pipeline (first == TRUE).
 * 

static void assign_fds_ext(t_minishell *minishell, t_node_cmd *tmp, int *flag)
{
    if (tmp->out_fd == -3)
    {
        // Output should be connected to a pipe
        tmp->out_fd = minishell->fd_pipes[1]; 	// assigns minishell->fd_pipes[1] (write end of the pipe) to tmp->out_fd.
    }
    else
    {
        // Output is redirected elsewhere, close the write end of the pipe
        close(minishell->fd_pipes[1]); 
        minishell->fd_pipes[1] = -2; // Indicate that the pipe is not needed
    }

    *flag = FALSE;
}
 */

/*
 * This function assigns file descriptors (in_fd and out_fd) for each command in the pipeline.
 * It iterates through the linked list of command nodes (t_node_cmd).
 * For the first command (first == TRUE), it calls assign_fds_ext.
 * For subsequent commands (first == FALSE), it assigns file descriptors based on whether input/output redirections are specified. 
 * It also updates the indices for minishell->fd_pipes.
 * If a command is the last in the pipeline (tmp->next == NULL), and the input file descriptor is set to -2, 
 * it means the input should come from a pipe. In this case, it sets tmp->in_fd to minishell->fd_pipes[minishell->nbr_pipes * 2 - 2].
 * The expression fd_pipes[minishell->nbr_pipes * 2 - 2] is indexing the array fd_pipes to obtain the file descriptor corresponding to the last pipe.
 * minishell->nbr_pipes: This is the number of pipes needed for the commands. Each pipe requires two file descriptors (one for reading, one for writing).
 * minishell->nbr_pipes * 2: This calculates the total number of file descriptors needed for all the pipes.
 * minishell->nbr_pipes * 2 - 2: Subtracting 2 gives the index of the last file descriptor in the array, as array indices are 0-based.
 * So, fd_pipes[minishell->nbr_pipes * 2 - 2] retrieves the file descriptor for the last pipe's read end. This file descriptor is used to connect the input of the last command to the output of the second-to-last command, effectively chaining the commands together through pipes.

static void assign_fds(t_minishell *minishell)  // moved: executor/cmd_redirect.c/fd_assign
{
    t_node_cmd *tmp;
    int first;
    int i;

    i = 0;
    tmp = minishell->cmds;
    first = TRUE;

    while (tmp)
    {
        // For the first command with a next command, handle output file descriptor
	if (first == TRUE && tmp->next)
            assign_fds_ext(minishell, tmp, &first);
        // For the last command, if input file descriptor is unassigned, connect it to the last pipe
		else if (first == FALSE && tmp->next == NULL && tmp->in_fd == -2)
            tmp->in_fd = minishell->fd_pipes[minishell->nbr_pipes * 2 - 2];
        // For subsequent commands, assign input and output file descriptors based on pipes
        else if (first == FALSE && tmp->next)
        {
            // If input file descriptor is unassigned, connect it to the current pipe
            if (tmp->in_fd == -2)
                tmp->in_fd = minishell->fd_pipes[i];
            // If output file descriptor is marked as -3, connect it to the next pipe
            if (tmp->out_fd == -3)
                tmp->out_fd = minishell->fd_pipes[i + 3];
            i += 2; // Move to the next pair of pipe file descriptors
        }
        tmp = tmp->next; // Move to the next command
    }
}
*/


////////////////////////OPEN FDs END (BEFORE PIPES)///////////////////////////


/*
int	is_builtin_with_output(char *str)
{
	if (ft_strncmp(str, "echo", 5) == 0 || ft_strncmp(str, "pwd", 4) == 0
		|| ft_strncmp(str, "env", 4) == 0)
		return (TRUE);
	return (FALSE);
}
*/

/*
 *
 * The waitpid system call is used to wait for a specific child process (pid) to terminate. 
 * The exit status of the process is stored in the status variable.
 * The macro WIFEXITED(status) checks whether the child process terminated normally (exited). 
 * If true (non-zero), the function returns the exit status using WEXITSTATUS(status).
 * If the child process did not terminate normally (e.g., it was terminated by a signal), 
 * the function returns a default exit status of 130. This value (130) is commonly used to indicate that the process was terminated 
 * by a user interrupt (e.g., pressing Ctrl+C), and it's a convention used in some shells.
 * 

int	ft_waitpid(pid_t pid) // moved to executor/cmd_execute.c
{
	int	status;

	// Wait for the specified process to terminate
	waitpid(pid, &status, 0);

	// Check if the process terminated normally (WIFEXITED)
	if (WIFEXITED(status) == 1)
		return (WEXITSTATUS(status));  // Return the exit status of the process

	// If the process did not terminate normally, return a default exit status (130)
	return (130);
}
*/

/*
 *
 * function checks whether each file descriptor (fd1 and fd2) is greater than 0, indicating that it is a valid file descriptor. 
 * If so, it closes the file descriptor using the close system call and updates the value to -2 or -3, depending on whether it's the first or second file descriptor.
 * The use of these specific values (-2 and -3) after closing the file descriptors might be indicative of some special meaning or state in the context of the program's design.
 * It's common in Unix-like systems to use negative values to represent certain states or flags.
 * 

void	ft_close_two_fd(int *fd1, int *fd2) // moved: executor/utils.c/ft_close_two_fd
{
	// If fd1 is a valid file descriptor, close it and update to -2
	if (*fd1 > 0)
	{
		close(*fd1);
		*fd1 = -2;
	}

	// If fd2 is a valid file descriptor, close it and update to -3
	if (*fd2 > 0)
	{
		close(*fd2);
		*fd2 = -3;
	}
}
*/

/*
 *
 * function is a signal handler for the SIGINT (interrupt) signal. 
 * It is designed to handle the SIGINT signal, which is typically sent to a process when the user presses Ctrl+C in the terminal.
 * Writes a newline character ("\n") to the standard output (file descriptor 1). This effectively moves the cursor to a new line in the terminal.
 * Uses the rl_replace_line("", 0) function from the Readline library to replace the current input line with an empty string.
 * Calls rl_on_new_line() to notify Readline that the cursor is now on a new line.
 * Calls rl_redisplay() to refresh the display in the terminal.


void ft_signal_handler(int signal) // moved: executor/signal_handler.c/clear_input_on_interrupt
{
    if (signal == SIGINT)
    {
        // Print a newline character
        write(1, "\n", 1);

        // Replace the current line with an empty line
        rl_replace_line("", 0);

        // Move the cursor to a new line
        rl_on_new_line();

        // Redisplay the prompt and line
        rl_redisplay();
    }
}

 */

/*
 * 
 * signal handler for the SIGINT signal in the child process. When the child process receives the SIGINT signal 
 * (usually when the user presses Ctrl+C in the terminal), this handler function will be called
 * In Unix-like systems, a process can exit with a specific status code. Conventionally,
 * a status code of 130 is often used to indicate that the process was terminated by an interrupt signal (e.g., Ctrl+C). 
 * The value 130 is derived from the standard exit code 128 plus the signal number (128 + 2, where SIGINT is signal number 2).
 * 

void	sig_handl_child(int signal) // moved: executor/signal_handler.c/exit_on_interrupt
{
	if (signal == SIGINT)
	{
		exit(130);
	}
}
*/


/*
 *
 * function designed to handle a here document (heredoc) in the context of command execution. 
 * A heredoc allows input to be provided directly within the script or command, typically terminated by a specific delimiter. 
 * The function sets up a pipe, reads lines from the user, writes them to the write end of the pipe, and redirects the read end 
 * of the pipe to the input file descriptor of the command.
 * t creates a pipe using the pipe system call. The pipe has two ends: fd[0] (read end) and fd[1] (write end).
 * It enters an infinite loop to read lines from the user using readline.
 * Inside the loop:
 * If the user enters a null line or a line matching the specified terminator (node->in_name), the loop is terminated.
 * Otherwise, the function writes the line followed by a newline character to the write end of the pipe (fd[1]).
 * After the loop, it checks if line is not null (indicating a successful readline), and if so, frees the allocated memory for line.
 * It closes the write end of the pipe (fd[1]) and the original input file descriptor of the command (node->in_fd).
 * It assigns the read end of the pipe (fd[0]) to the input file descriptor of the command (node->in_fd), establishing the connection between the heredoc and the command.
 * 

int	heredoc_function(t_node_cmd *node) // moved to executor/cmd_execute.c
{
	int		fd[2];
	char	*line;

	if (pipe(fd) == -1)
		return (ft_strerror(strerror(errno), "heredoc pipe"), -1);
	else
	{
		while (1)
		{
			line = readline("heredoc> ");
			if (line == NULL || (ft_strncmp(line, node->in_name,
						ft_strlen(node->in_name) + 1) == 0))
				break ;
			write(fd[1], line, ft_strlen(line));
			write(fd[1], "\n", 1);
		}
		if (line)
			free(line);
		ft_close_two_fd(&fd[1], &node->in_fd);
		node->in_fd = fd[0];
	}
	return (0);
}
*/
/*
static int	get_path(t_minishell *minishell, t_node_cmd *node)  // moved to executor/cmd_execute.c
{
	char	*tmp;
	int		i;

	i = 0;

	// Check if the command name is valid
	if (node->cmd[0] == NULL || ft_strlen(node->cmd[0]) == 0)
		return (FALSE);

	// Iterate through the directories in minishell->path
	while (minishell->path && minishell->path[i])
	{
		// Construct the full path for the executable
		tmp = ft_strjoin(minishell->path[i++], "/");
		node->path = ft_strjoin(tmp, node->cmd[0]);
		free(tmp);

		// Check if the constructed path is executable
		if (access(node->path, X_OK) == 0)
			return (TRUE);

		// Free the memory allocated for the current path
		free(node->path);
		node->path = NULL;
	}

	// Return FALSE if no valid path is found for the executable
	return (FALSE);
}
*/
/*
 *
 * function is responsible for executing a command using the execve system call.
 * This function is a crucial part of command execution, as it replaces the current process image with a new one based on the specified command and arguments.
 * 
 * ADD MINISHELL free

void	ft_execve(t_node_cmd *node, t_minishell *minishell, char **env)  // moved to executor/cmd_execute.c
{
	// If the path is not set and cannot be obtained, display an error and exit
	if (node->path == NULL && get_path(minishell, node) == FALSE)
	{
		ft_strerror("command not found", node->cmd[0]);
		ft_close_two_fd(&node->in_fd, &node->out_fd);
		exit(127);
	}

	// Execute the command using execve
	if (execve(node->path, node->cmd, env) == -1)
	{
		// If execve fails, display an error and exit
		ft_close_two_fd(&node->in_fd, &node->out_fd);
		ft_strerror(strerror(errno), node->cmd[0]);
		exit(126);
	}
}
*/

/*
 *
 * function is responsible for executing built-in commands in a child process. 
 * It checks the command name and calls the corresponding built-in function.
 * The purpose of forking a child process for built-ins is to isolate their execution and avoid affecting the main shell process.
 * The rest of the builtins - the ones that have output to console
 * 

void	ft_execute_fork_builtin(t_minishell *minishell, t_node_cmd *cmd)  // moved to executor/cmd_execute.c
{
	// Check if the command is "echo"
	if (ft_strncmp(cmd->cmd[0], "echo", 5) == 0)
		ft_echo(cmd);
	// Check if the command is "pwd"
	else if (ft_strncmp(cmd->cmd[0], "pwd", 4) == 0)
		ft_pwd(cmd);
	// If the command is not "echo" or "pwd", assume it's "env"
	else
		ft_env(minishell);

	// Exit the child process with status 0
	exit(0);
}
*/

/*
 * function that handles input and output redirection before executing the command in the child process.

static int execute_cmd_ext(t_node_cmd *node, int is_builtin)  // moved to executor/cmd_execute.c
{
    // If input file descriptor is negative, duplicate standard input
    if (node->in_fd < 0)
        node->in_fd = dup(STDIN_FILENO);

    // If output file descriptor is negative, duplicate standard output
    if (node->out_fd < 0)
        node->out_fd = dup(STDOUT_FILENO);

    // Handle here document (heredoc) if redirection is "<<"
    if (node->redir_in && ft_strncmp(node->redir_in, "<<", ft_strlen("<<") + 1) == 0)
        heredoc_function(node);

    // If it's not a builtin command, perform input redirection
    if (is_builtin == FALSE)
    {
        if (dup2(node->in_fd, STDIN_FILENO) < 0)
            return (ft_strerror(strerror(errno), "dup2 error"), -2);
    }
    else
    {
        // If it's a builtin command, close the input file descriptor
        if (node->in_fd > 0)
            close(node->in_fd);
    }

    // Perform output redirection
    if (dup2(node->out_fd, STDOUT_FILENO) < 0)
        return (ft_strerror(strerror(errno), "dup2 error"), -3);

    return (0);
}
*/

/*
 * function forks a child process using fork() and then executes the command in the child process.
 * signal handler for the SIGINT signal in the child process. This means that when the child process receives the SIGINT signal 
 * (which is typically triggered by pressing Ctrl+C in the terminal), the sig_handl_child function will be called.
 * signal(SIGINT, SIG_IGN); line is used to ignore the SIGINT signal in the current process. 
 * This means that if the process receives a SIGINT signal (typically sent when the user presses Ctrl+C in the terminal),
 * the default action of terminating the process will be ignored, and the process will continue running.

int execute_cmd(t_node_cmd *node, t_minishell *minishell, char **env)  // moved to executor/cmd_execute.c
{
    pid_t pid;
    int is_builtin;
    int exit_status;

    // Check if the command is a builtin command that produces output
    is_builtin = is_builtin_with_output(node->cmd[0]);

    // Fork a child process
    pid = fork();
    if (pid < 0)
        return (ft_strerror(strerror(errno), "fork"), -1);
    else if (pid == 0)
    {
        // Child process
        signal(SIGINT, sig_handl_child);

        // Execute the command in the child process
        if (execute_cmd_ext(node, is_builtin) != 0)
            return (-1);

        // If it's a builtin command, execute it in the child process
        if (is_builtin)
            ft_execute_fork_builtin(mini, node);

        // Execute the command using execve
        ft_execve(node, mini, env);
    }

    // Parent process
    signal(SIGINT, SIG_IGN);

    // Close the input and output file descriptors in the parent process
    ft_close_two_fd(&node->in_fd, &node->out_fd);

    // Wait for the child process to finish and get the exit status
    exit_status = ft_waitpid(pid);

    // Restore the signal handler for SIGINT in the parent process
    signal(SIGINT, ft_signal_handler);

    return (exit_status);
}
*/

/*
 * This function executes a single command (t_node_cmd) based on whether it is a built-in command or an external command.
 * If it is a built-in command and does not produce output (!is_builtin_with_output(tmp->cmd[0])), it calls ft_execute_builtin to handle the built-in command execution.
 * If it is an external command, it calls execute_cmd to execute the command.
 * 

static void	ft_executor_ext_two(t_minishell *minishell, t_node_cmd *tmp, char **envp) // moved to executor/executor.c
{
	if (ft_is_builtin(tmp->cmd[0]) && !is_builtin_with_output(tmp->cmd[0]))
		minishell->exitstatus = ft_execute_builtin(mini, tmp);
	else
		minishell->exitstatus = execute_cmd(tmp, mini, envp);
	signal(SIGINT, ft_signal_handler);
}
 */


/*
 *  This function is responsible for the initial setup before executing commands.
 *  It calls create_pipes to set up pipes for communication between commands. 
 *  If create_pipes fails, it sets minishell->exitstatus to 1 and returns -1.
 *  It then calls open_redirs to handle input/output redirections.
 *  Finally, it calls assign_fds to assign file descriptors for each command in the pipeline.
 *

static int	ft_executor_ext_one(t_minishell *minishell)  // moved to executor/executor.c
{
	// Create pipes and check for errors
	if (create_pipes(mini) == -1)
	{
		minishell->exitstatus = 1;
		return (-1);
	}

	// Open input/output file descriptors based on redirections
	open_redirs(mini);

	// Assign file descriptors to commands
	assign_fds(mini);

	return (0);
}
*/

/*
 *
 * Function closes all the file descriptors associated with pipes in the mini structure
 * The function uses a loop to iterate through the array minishell->fd_pipes, which contains file descriptors associated with pipes.
 * For each iteration, it calls the ft_close_two_fd function to close the read and write ends of the pipe. This is done by passing the addresses of the current pair of file descriptors.
 * The loop increments i by 2 in each iteration to move to the next pair of file descriptors in the array.

void ft_close_pipes(t_minishell *minishell)  // moved to executor/executor.c
{
    int i;

    i = 0;

    // Iterate through the array of pipe-related file descriptors
    while (i < minishell->nbr_pipes * 2)
    {
        // Close the read and write ends of the pipe
        ft_close_two_fd(&minishell->fd_pipes[i], &minishell->fd_pipes[i + 1]);

        // Move to the next pair of file descriptors
        i += 2;
    }
}
*/

/*
 *	main entry point for executing commands in a shell or command-line interpreter
 *
 *	The function begins by checking if the preparation for execution (setting up pipes and redirections) was successful by calling ft_executor_ext_one. 
 *	If not, it returns early.
 * It then iterates through the linked list of command nodes (t_node_cmd). For each node:
 * It checks for conditions where the command execution should be skipped:
 * If the command is empty (!tmp->cmd[0]), or
 * If the command has a valid input file descriptor (tmp->in_fd == -1).
 * If the special case for HEREDOC error with an empty command is detected, it prints an error message using ft_strerror and sets the exit status to 1.
 * It closes the input and output file descriptors using ft_close_two_fd and moves to the next command node.
 * If the conditions are met for executing the command, it calls ft_executor_ext_two to execute the command and handle redirections.
 * After processing all command nodes, it closes any remaining pipes using ft_close_pipes.

void ft_executor(t_minishell *minishell, char **envp)  // moved to executor/executor.c
{
    t_node_cmd *tmp;

    // Check if preparation for execution (pipes and redirections) was successful
    if (ft_executor_ext_one(minishell) != 0)
        return;

    // Iterate through the linked list of command nodes
    tmp = minishell->cmds;
    while (tmp)
    {
        // Check for conditions where the command execution should be skipped
        if (!tmp->cmd[0] || (tmp->cmd[0] && tmp->in_fd == -1))
        {
            // Handle special case for HEREDOC error with an empty command
            if (!tmp->cmd[0] && tmp->redir_in
                && ft_strncmp(tmp->redir_in, "<<", ft_strlen("<<") + 1) == 0)
            {
                ft_strerror("HEREDOC error", "empty command");
                minishell->exitstatus = 1;
            }

            // Close file descriptors and move to the next command node
            ft_close_two_fd(&tmp->in_fd, &tmp->out_fd);
            tmp = tmp->next;
            continue;
        }

        // Execute the command and handle redirections
        ft_executor_ext_two(minishell, tmp, envp);

        // Move to the next command node
        tmp = tmp->next;
    }

    // Close any remaining pipes
    ft_close_pipes(minishell);
}
 */


/*
 *
 * Function, which creates a duplicate (copy) of the input string s1 up to a specified maximum length n.

char *ft_strndup(char *s1, int n)
{
    char *copy;
    size_t s1_len;
    int i;

    i = 0;

    // Calculate the length of the input string s1
    s1_len = ft_strlen(s1);

    // Allocate memory for the copy with space for the null-terminator
    copy = malloc(sizeof(char) * (s1_len + 1));
    if (!copy)
        return (NULL);

    // Copy characters from s1 to the new copy, up to the specified maximum length n
    while (s1[i] && i < n)
    {
        copy[i] = s1[i];
        i++;
    }

    // Null-terminate the copy
    copy[i] = '\0';

    // Return the duplicated string
    return (copy);
}
*/


/*
 * constructing a linked list of tokens during the tokenization process

t_elem	*new_elem(char *content, int len, enum e_token type, enum e_state state)
{
	t_elem	*elem;
	char	*cont;

	elem = ft_calloc(sizeof(t_elem), 1);
	if (!elem)
		return (NULL);
	cont = ft_strndup(content, len);
	if (!cont)
		return (NULL);
	elem->content = cont;
	elem->len = len;
	elem->type = type;
	elem->state = state;
	return (elem);
}

 */

/*
static int	is_empty(t_list_n *list)
{
	return (list->head == NULL);
}

 */

/*
 * mechanism to manage a doubly-linked list (t_list_n). 
 * It updates the tail of the list and adjusts pointers to add a new element to the end of the list. 

void	add_tail(t_list_n *list, t_elem *new)
{
	if (is_empty(list))
		list->head = new;
	else
	{
		list->tail->next = new;
		new->prev = list->tail;
	}
	list->tail = new;
	list->size++;
}
 */

/*
 * handle cases where redirection symbols ('<' or '>') are encountered in the input string.
 * It creates the appropriate tokens based on the type of redirection and updates the index accordingly.
 *  The tokens are added to the linked list for further processing in the lexer.
 *

int make_token_redir(t_list_n *tokens, char *str, int idx, enum e_state state)  // moved to parser/tokenize.c/
{
    int i = idx;

    // Check if the current character is '<' (input redirection)
    if (str[idx] == '<')
    {
        // Check if the next character is also '<' (here document)
        if (str[idx + 1] && str[idx + 1] == '<')
            add_tail(tokens, new_elem(str + idx++, 2, HERE_DOC, state));
        else
            add_tail(tokens, new_elem(str + idx, 1, REDIR_IN, state));
        
        idx++;  // Increment the index to move to the next character
    }
    // Check if the current character is '>' (output redirection)
    else if (str[idx] == '>')
    {
        // Check if the next character is also '>' (double output redirection)
        if (str[idx + 1] && str[idx + 1] == '>')
            add_tail(tokens, new_elem(str + idx++, 2, DREDIR_OUT, state));
        else
            add_tail(tokens, new_elem(str + idx, 1, REDIR_OUT, state));
        
        idx++;  // Increment the index to move to the next character
    }

    // Return the difference between the original and updated indices
    return (idx - i);
}
 */

/*
static int	is_alphanum(int c)
{
	if ((c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z'))
		return (TRUE);
	return (FALSE);
}
*/

/*
 * handle the case where an environment variable is encountered in the input string.
 * If the variable is a special variable (like $? or $0, $1, etc.), it increments the index accordingly. 
 * If it's a regular alphanumeric variable, it continues processing until a non-alphanumeric character is found.
 *

int get_env_token(t_list_n *tokens, char *str, enum e_state state)  // moved to parser/tokenize.c/
{
    int i = 1;

    // Check if the next character is either '?' or a digit (0-9)
    if (str[i] == '?' || (str[i] >= '0' && str[i] <= '9'))
        i++;
    else
    {
        // If not, continue processing alphanumeric characters until a non-alphanumeric character is encountered
        while (is_alphanum(str[i]) && str[i] != '\n' && str[i] != '\0')
            i++;
    }

    // Add a new token to the linked list representing the environment variable
    add_tail(tokens, new_elem(str, i, ENV, state));

    return i;
}
*/

/*
 * This function handles the state transitions between quotes (') and double quotes (").
 * It checks the current lexer state and decides whether to start, end, or continue a quote state.
 * The function adds a new token to the linked list (tokens) based on the determined state and token type.
 *

void quotes_state(t_list_n *tokens, char *str, enum e_state *state, int flag)  // moved to parser/tokenize.c/
{
    enum e_state e_state;
    enum e_token e_type;

    // Determine the state and token type based on the flag
    if (flag == 1)
    {
        e_state = IN_QUOTE;
        e_type = QOUTE;
    }
    else
    {
        e_state = IN_DQUOTE;
        e_type = DOUBLE_QUOTE;
    }

    // Check the current lexer state and handle accordingly
    if (*state == GENERAL)
    {
        // If in the GENERAL state, add a new token representing the quote to the list
        add_tail(tokens, new_elem(str, 1, e_type, *state));
        *state = e_state;  // Update the lexer state to the determined state
    }
    else if (*state == e_state)
    {
        // If already in the determined state, close the quote and return to the GENERAL state
        *state = GENERAL;
        add_tail(tokens, new_elem(str, 1, e_type, *state));
    }
    else
    {
        // If in a different state, add a new token representing the quote to the list
        add_tail(tokens, new_elem(str, 1, e_type, *state));
    }
}
*/

/*
static int	ft_isspace(char c)
{
	if (c == ' ' || c == '\t' || c == '\v'
		|| c == '\r' || c == '\f' || c == '\n')
		return (1);
	return (0);
}

static int	in_charset(char c)
{
	return (c == '\'' || c == '\"' || c == '<' || c == '>' || c == '|'
		|| c == '$' || ft_isspace(c) || c == 0);
}
*/

/*
 * processes a word in the input string until a non-word character is encountered.
 * It adds a new token to the linked list (tokens) representing the word.

int get_word(t_list_n *tokens, char *str, enum e_state state)  // moved to parser/tokenize.c/tokenize
{
    int i = 0;

    // Count the length of the word until a non-word character is encountered
    while (!in_charset(str[i]))
        i++;

    // Add a new token to the linked list representing the word
    add_tail(tokens, new_elem(str, i, WORD, state));
    return i;
}
*/

/*
 * function effectively drives the tokenization process,
 * handling different cases based on the type of character encountered
 * and updating the linked list of tokens accordingly.

int ft_make_token(t_list_n *tokens, char *str, int i, enum e_state *state)  // moved to parser/tokenize.c/tokenize
{
    // Check if the current character is part of a word
    if (in_charset(str[i]) != 1)
        i += get_word(tokens, str + i, *state);
    // Check if the current character is a single quote
    else if (str[i] == '\'')
        quotes_state(tokens, str + i++, state, 1);
    // Check if the current character is a double quote
    else if (str[i] == '"')
        quotes_state(tokens, str + i++, state, 2);
    // Check if the current character is a dollar sign ('$')
    else if (str[i] == '$')
    {
        // Check if the next character is a special case or a regular character
        if (str[i + 1] && in_charset(str[i + 1]))
            add_tail(tokens, new_elem(str + i++, 1, WORD, *state));
        else
            i += get_env_token(tokens, str + i, *state);
    }
    // Check if the current character is a redirection symbol ('>' or '<')
    else if (str[i] == '>' || str[i] == '<')
        i += make_token_redir(tokens, str, i, *state);
    // Check if the current character is a pipe symbol ('|')
    else if (str[i] == '|')
        add_tail(tokens, new_elem(str + i++, 1, PIPE_LINE, *state));
    // Check if the current character is a whitespace character
    else if (ft_isspace(str[i]))
        add_tail(tokens, new_elem(str + i++, 1, WHITE_SPACE, *state));

    return i;
}
*/

/*
t_list_n	*init_list(t_list_n *list)
{
	list = ft_calloc(sizeof(t_list_n), 1);
	if (!list)
		return (NULL);
	return (list);
}
 */

/*
 * simple lexer (lexical analyzer) using a linked list data structure to represent tokens. 

t_list_n *lexer(char *line)  // moved to parser/lexer.c
{
    t_list_n *tokens;    // Linked list to store tokens
    int i;               // Index for iterating through the input line
    enum e_state state;  // Current state of the lexer (enum e_state)

    i = 0;
    state = GENERAL;     // Initialize lexer state to GENERAL
    tokens = NULL;       // Initialize tokens to NULL
    tokens = init_list(tokens);  // Initialize the linked list

    if (tokens)
    {
        while (line[i])  // Iterate through the characters of the input line
            i = ft_make_token(tokens, line, i, &state);  // Process characters and create tokens
    }

    return (tokens);  // Return the linked list of tokens
}
 */

/*
 *
 * ptr is a pointer to a token (t_elem) representing the current position in the token list.
 * opt is an integer parameter that determines the direction in which to skip space. If opt is non-zero (true), it means skipping forward (using ptr->next), otherwise, it means skipping backward (using ptr->prev).
 * The function uses a while loop to iterate through consecutive white space tokens. Inside the loop:
 * If ptr is not NULL and the type of the token is WHITE_SPACE, it continues to skip over white space.
 * If opt is non-zero, it moves ptr to the next token (ptr->next), else it moves ptr to the previous token (ptr->prev).
 * The loop continues until a non-white space token is encountered or ptr becomes NULL.
 * Finally, the function returns the updated position of the pointer in the token list after skipping over white space tokens.
 * This updated pointer can be used by the caller to access the next non-white space token in the specified direction.

t_elem	*skip_space(t_elem *ptr, int opt)  // moved to parser/syntax_errors.c
{
	while (ptr && ptr->type == WHITE_SPACE)
	{
		if (opt)
			ptr = ptr->next;
		else
			ptr = ptr->prev;
	}
	return (ptr);
}

*/

/*
 *
 * This function checks for syntax errors related to pipes (|).
 * It looks at the previous and next tokens, skipping any leading/trailing spaces using the skip_space function.
 * If either the previous or next token is missing, it returns 1 (indicating an error)

int	pipe_err(t_elem *el)
{
	t_elem	*prev;
	t_elem	*next;

	prev = skip_space(el->prev, 0);
	next = skip_space(el->next, 1);
	if (!prev || !next)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}


 *
 * This function checks for syntax errors in redirections (e.g., <, >, >>).
 * It looks at the next token after the redirection symbol and ensures it is of a valid type (WORD, ENV, DOUBLE_QUOTE, or QOUTE).

int	redir_err(t_elem *ptr)
{
	t_elem	*next;

	next = skip_space(ptr->next, 1);
	if (!next || (next->type != WORD && next->type != ENV
			&& next->type != DOUBLE_QUOTE && next->type != QOUTE))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}


 *
 * function checks for unclosed quotes (single or double quotes).
 * It iterates through the tokens until it finds a token of the specified type or reaches the end of the list.
 * If it doesn't find the closing quote, it prints an error message and returns the position where the unclosed quotes were detected.
 * Inside the loop:
 * If *ptr is NULL or the type of the token is equal to the specified type (which represents the type of the closing quote), the loop breaks.
 * After the loop, if *ptr is still NULL, it means the closing quote was not found, and an error message is written to the standard error

t_elem	*quotes_err(t_elem **ptr, enum e_token type)
{
	while (*ptr)
	{
		*ptr = (*ptr)->next;
		if (!*ptr || (*ptr)->type == type)
			break ;
	}
	if (!*ptr)
		write(2, "unclosed quotes detected.\n",
			ft_strlen("unclosed quotes detected.\n"));
	return (*ptr);
}
*/

/*
 *
 *  function returns a string representation of the redirection symbol associated with the given type.
 * It uses ft_strdup to create a duplicate of the string.

char	*get_redir_symb(enum e_token type)  / moved to parser/utils.c
{
	if (type == REDIR_IN)
		return (ft_strdup("<"));
	else if (type == REDIR_OUT)
		return (ft_strdup(">"));
	if (type == HERE_DOC)
		return (ft_strdup("<<"));
	if (type == DREDIR_OUT)
		return (ft_strdup(">>"));
	return (NULL);
}

*/

/*
 *
 * ft_perror takes two parameters: msg (the main error message) and utils (additional information or a specific character, possibly a pipe character).
 * It uses the write function to output the main error message and additional information (if provided) to the standard error output.
 * It checks if utils is not NULL before attempting to use it.
 * If utils is not NULL and doesn't represent a pipe character ("|"), it frees the allocated memory for utils 

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

*/

/*
int	is_redir(enum e_token type) // moved to parser/utils.c
{
	return (type == REDIR_IN || type == REDIR_OUT
		|| type == DREDIR_OUT || type == HERE_DOC);
}
*/

/*
 *
 * iterates through the linked list of tokens (t_elem) using a temporary pointer tmp.
 * For each token, it checks for syntax errors related to pipes, redirections, and quotes.
 * If a syntax error is found, it returns an error message using the ft_perror function.

int sintax_check(t_list_n *list) // moved to parser/syntax_check.c
{
    t_elem *tmp;

    tmp = list->head;
    while (tmp)
    {
        // Check for pipe syntax errors
        if (tmp->type == PIPE_LINE)
        {
            if (pipe_err(tmp))
                return (ft_perror("Syntax error: unexpected token near ", "|"));
        }

        // Check for redirection syntax errors
        if (is_redir(tmp->type))
        {
            if (redir_err(tmp))
                return (ft_perror("Syntax error in redirection ", get_redir_symb(tmp->type)));
        }
        else if (tmp->type == DOUBLE_QUOTE || tmp->type == QOUTE)
        {
            // Check for quotes syntax errors
            if (!quotes_err(&tmp, tmp->type))
                return (ft_perror("Syntax error in quotes", NULL));
        }

        tmp = tmp->next;
    }

    return (0);
}
 */

/*
static int	ft_has_characters(char *line) // moved to parser/utils.c/is_any_printable
{
	int	i;

	i = 0;
	if (line)
	{
		while (line[i])
		{
			if (line[i] > 32)
				return (1);
			i++;
		}
	}
	return (0);
}
*/

/*
 *
 * The get_pipe_nbr function takes a linked list of elements (t_list_n) and counts the number of pipe tokens (PIPE_LINE) in the list.
 * It iterates through the elements and increments the count 

static int	get_pipe_nbr(t_list_n *list)  // moved to parser/parse.c  ** will be moved to token_utils
{
	int		n;
	t_elem	*tmp;

	n = 0;
	tmp = list->head;
	while (tmp)
	{
		if (tmp->type == PIPE_LINE)
			n++;
		tmp = tmp->next;
	}
	return (n);
}
*/

/*
 *
 * The function starts by initializing a pointer (current) to the head of the environment list.
 * It then iterates through the linked list (while (current)) and compares the key of each node with the provided key using ft_strncmp.
 * If a match is found, it returns a duplicated copy of the corresponding value using ft_strdup.
 * If no match is found, it returns an empty string (allocated using ft_calloc).
 * This function is used within the parse_env function to retrieve the value of environment variables when parsing ENV tokens. If the environment variable is found, its value is returned; otherwise, an empty string is returned.

char *get_env(t_node *env, char *key)
{
    t_node *current = env;

    while (current)
    {
        // Compare the key of the current node with the provided key
        if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
        {
            // If a match is found, return a duplicated copy of the value
            return (ft_strdup(current->value));
        }
        // Move to the next node in the list
        current = current->next;
    }

    // If no match is found, return an empty string
    return (ft_calloc(1, 1));
}
*/

/*
 *
 * special case when the environment variable is "$?". It converts the exit status to a string and stores it in the args array.

static void parse_env_ext(int ex_stat, char **args, int *i)
{
    char *tmp = ft_itoa(ex_stat);
    args[*i] = ft_strdup(tmp);
    free(tmp);
}

void parse_env(t_elem **elem, char **args, int *i, t_minishell *minishell)  // moved to parser/parse.c
{
    char *tmp;

    if (ft_strncmp((*elem)->content, "$?", ft_strlen("$?")) == 0)
    {
        // Special case: $?
        parse_env_ext(minishell->exitstatus, args, i);
    }
    else
    {
        // Regular environment variable
        tmp = get_env(minishell->enviro, (*elem)->content + 1);
        if (tmp == NULL)
        {
            // If the environment variable is not found, treat it as a regular word
            args[*i] = ft_strdup((*elem)->content);
            if ((*elem)->next && (*elem)->next->type == WORD)
            {
                // If the next token is a WORD, concatenate it to the current argument
                tmp = args[*i];
                args[*i] = ft_strjoin(tmp, (*elem)->next->content);
                free(tmp);
                (*elem) = (*elem)->next;
            }
        }
        else
        {
            // If the environment variable is found, store its value in the args array
            args[*i] = tmp;
        }
    }

    // Move to the next token in the list
    if (args[*i])
        (*i)++;
    (*elem) = (*elem)->next;
}
*/

/*
void parse_word(t_elem **elem, char **args, int *i)  // moved to parser/parse.c
{
    // Extract the content of the WORD token and store it in the args array
    args[*i] = ft_substr((*elem)->content, 0, (*elem)->len + 1);
    (*i)++;
    
    // Move to the next token in the list
    (*elem) = (*elem)->next;
}
*/

/*
 *
 *  It concatenates the value of the environment variable (val) to the current line variable.

static char	*parse_quot_ext_ext(t_elem **node, char **val, char **line)
{
	char	*tmp;

	tmp = NULL;
	if (*val)
	{
		tmp = ft_strjoin(*line, *val);
		free(*val);
		*val = NULL;
	}
	else
	{
		*val = ft_strjoin(*line, (*node)->content);
		if ((*node)->next && (*node)->next->type == WORD)
		{
			tmp = ft_strjoin(*val, (*node)->next->content);
			free(*val);
			*val = NULL;
			(*node) = (*node)->next;
		}
	}
	return (tmp);
}
*/


/*
 *
 * s responsible for handling the content inside quotes. 
 * It checks if the current token is an environment variable (ENV) and if it is within double quotes (IN_DQUOTE). 
 * If so, it retrieves the value of the environment variable and appends it to the line variable.

static void	parse_quot_ext(t_elem **node, char **val, char **line, t_minishell *minishell)
{
	char	*tmp;

	tmp = NULL;
	if ((*node)->type == ENV && (*node)->state == IN_DQUOTE)
	{
		if (ft_strncmp((*node)->content, "$?", ft_strlen("$?")) == 0)
			*val = ft_itoa(minishell->exitstatus);
		else
			*val = get_env(minishell->enviro, (*node)->content + 1);
		tmp = parse_quot_ext_ext(node, val, line);
	}
	else
		tmp = ft_strjoin(*line, (*node)->content);
	free(*line);
	*line = tmp;
}
*/

/*
 *
 * designed to parse the content within quotes (QOUTE or DOUBLE_QUOTE). 
 * It appends the content within the quotes to the line variable and handles special cases where the content inside quotes includes environment variables.
 * The val variable is used to store the value of environment variables if they are present inside the quotes.

char	*parse_quo(char *line, t_elem **node, enum e_token type, t_minishell *minishell)  // moved to parser/parse.c
{
	char	*val;

	val = NULL;
	*node = (*node)->next;
	if ((*node) && (*node)->type == type)
	{
		if (!line)
			line = ft_strdup("");
		*node = (*node)->next;
		return (line);
	}
	while ((*node) && (*node)->type != type)
	{
		if (!line)
			line = ft_strdup("");
		parse_quot_ext(node, &val, &line, mini);
		*node = (*node)->next;
	}
	if (val)
		free(val);
	if (*node)
		*node = (*node)->next;
	return (line);
}
*/

/*
 * etrieves the name (path or file descriptor) associated with a redirection.
 * It considers different cases, such as quotes (DOUBLE_QUOTE or QOUTE), environment variables (ENV), and regular words (WORD).
 *

char	*get_name(t_elem **node, t_minishell *minishell)  // moved to parser/parse_redir.c/get_redir_dest
{
	char	*tmp;

	tmp = NULL;
	if ((*node)->type == DOUBLE_QUOTE || (*node)->type == QOUTE)
		tmp = parse_quo(tmp, node, (*node)->type, mini);
	else if ((*node)->type == ENV)
	{
		tmp = get_env(minishell->enviro, (*node)->content + 1);
		if (*node)
			*node = (*node)->next;
	}
	else if ((*node)->type == WORD)
	{
		tmp = ft_strdup((*node)->content);
		if (*node)
			*node = (*node)->next;
	}
	return (tmp);
}
*/

/*
 *
 * Function checks if a redirection is already set in a t_node_cmd structure and clears the associated data if needed. 
 * It's called before parsing a new redirection to avoid conflicts.

void	check_redir(t_node_cmd *new, enum e_token type)  //moved to parser/parse_redir.c/rm_redir_if_any
{
	if (new->redir_in && (type == REDIR_IN || type == HERE_DOC))
	{
		free(new->in_name);
		new->in_name = NULL;
		free(new->redir_in);
		new->redir_in = NULL;
	}
	else if ((new->redir_out && type == REDIR_OUT)
		|| (new->redir_out && type == DREDIR_OUT))
	{
		free(new->out_name);
		new->out_name = NULL;
		free(new->redir_out);
		new->redir_out = NULL;
	}
}
*/

/*
 *
 * edirection token (REDIR_IN, REDIR_OUT, DREDIR_OUT, HERE_DOC) and sets the appropriate fields in the t_node_cmd structure.
 * It also retrieves the associated name using the get_name function.

void	parse_redir(t_elem **node, t_minishell *minishell, t_node_cmd	*new)  // moved to parser/parse_redir.c/parse_redir
{
	enum e_token	type;
	char			*tmp;

	type = (*node)->type;
	check_redir(new, type);
	if (type == REDIR_OUT || type == DREDIR_OUT)
		new->redir_out = get_redir_symb(type);
	else
		new->redir_in = get_redir_symb(type);
	while ((*node)->type != WORD && (*node)->type != ENV
		&& (*node)->type != DOUBLE_QUOTE && (*node)->type != QOUTE)
		*node = (*node)->next;
	tmp = get_name(node, mini);
	if (type == REDIR_OUT || type == DREDIR_OUT)
		new->out_name = tmp;
	else
		new->in_name = tmp;
}
*/

/*
 *
 * function initializes a new t_node_cmd structure. The parameters are the size of the command (number of arguments) and
 * a pointer i that will be used to keep track of the current index while populating the cmd array.
 * The function allocates memory for the structure and its arrays (cmd) and initializes various fields.

t_node_cmd	*new_cmd_node_init(int size, int *i)
{
	t_node_cmd	*new;

	*i = 0;
	new = ft_calloc(sizeof(t_node_cmd), 1);
	if (new == NULL)
		return (NULL);
	new->path = NULL;
	new->cmd = ft_calloc((size + 1), sizeof(char *));
	if (new->cmd == NULL)
		return (NULL);
	new->redir_in = NULL;
	new->in_name = NULL;
	new->in_fd = -2;
	new->redir_out = NULL;
	new->out_name = NULL;
	new->out_fd = -3;
	new->next = 0;
	return (new);
}
*/


/*
 *
 * iterates through the input list, parsing tokens based on their types. 
 * It handles different token types, such as WORD, ENV, WHITE_SPACE, DOUBLE_QUOTE, QOUTE, and redirection tokens 

t_node_cmd *new_cmd_node(t_elem **list, int size, t_minishell *minishell)  // moved to parser/token2cmd.c/cmd_from_token.c
{
    t_node_cmd *new;
    int i;

    // Initialize a new t_node_cmd structure
    new = new_cmd_node_init(size, &i);
    if (new == NULL)
        return (NULL);

    // Loop through the list until a PIPE_LINE token is encountered
    while ((*list) && (*list)->type != PIPE_LINE)
    {
        // Check the type of the current token and perform parsing accordingly
        if ((*list)->type == WORD)
            parse_word(list, new->cmd, &i);
        else if ((*list)->type == ENV)
            parse_env(list, new->cmd, &i, mini);
        else if ((*list)->type == WHITE_SPACE)
            (*list) = (*list)->next;
        else if ((*list)->type == DOUBLE_QUOTE || (*list)->type == QOUTE)
        {
            new->cmd[i] = parse_quo(new->cmd[i], list, (*list)->type, mini);
            if (new->cmd[i])
                i++;
        }
        else if (is_redir((*list)->type))
            parse_redir(list, mini, new);
    }

    // Terminate the cmd array with NULL
    new->cmd[i] = NULL;
    return (new);
}
*/


/*
 *
 * Increments the argument count (*i) based on the type of the token. 
 * For quotes, it iterates until it finds the closing quote. For redirection tokens, it skips to the next WORD or ENV token.
 *  while loop iterates through the list of t_elem elements using the pointer (*elem).
 *  The loop continues as long as (*elem) is not NULL (there are more elements) and the type of the current element (*elem)->type is not equal to the specified type.
 *  The goal is to find the closing quote of the quoted section.

void	count_quotes_redir_args(t_elem **elem, enum e_token type, int *i)  // moved to parser/parse.c/count_args (new merged function count_quotes_redir_args+count_args)
{
	if (type == DOUBLE_QUOTE || type == QOUTE)
	{
		(*elem) = (*elem)->next;
		while ((*elem) && (*elem)->type != type)
			(*elem) = (*elem)->next;
		(*i)++;
	}
	else if (is_redir(type))
	{
		while ((*elem)->type != WORD && (*elem)->type != ENV)
			(*elem) = (*elem)->next;
		(*elem) = (*elem)->next;
	}
}
*/

/*
 *
 * counts the number of arguments until it encounters a PIPE_LINE token.
 * It considers different token types such as WORD, ENV, DOUBLE_QUOTE, QOUTE, and redirection tokens (is_redir). 

int	count_args(t_elem *elem)  // moved to parser/parse.c/count_args (new merged function count_quotes_redir_args+count_args)
{
	t_elem	*tmp;
	int		i;

	i = 0;
	tmp = elem;
	while (tmp && tmp->type != PIPE_LINE)
	{
		if (tmp->type == WORD || tmp->type == ENV)
			i++;
		else if (tmp->type == DOUBLE_QUOTE || tmp->type == QOUTE
			|| is_redir(tmp->type))
		{
			count_quotes_redir_args(&tmp, tmp->type, &i);
			if (tmp)
				tmp = tmp->next;
			continue ;
		}
		if (tmp)
			tmp = tmp->next;
	}
	return (i);
}
*/

/*
 *
 * or building a linked list of t_node_cmd nodes, where each node represents a command in the execution sequence of a shell.
 * The linked list is extended as new commands are encountered.

static void ft_add_cmd_end(t_node_cmd **head, t_node_cmd *new)
{
    t_node_cmd *current;

    // If the linked list is empty, set the new node as the head
    if (*head == NULL)
    {
        *head = new;
        return;
    }

    // Traverse the linked list to find the last node
    current = *head;
    while (current->next != NULL)
        current = current->next;

    // Append the new node to the end of the list
    current->next = new;
}
*/

/*
 *
 * function takes a linked list of elements (t_list_n) and a data structure (t_minishell *minishell) as parameters.
 * It iterates through the elements of the linked list, counts the number of arguments in each command, creates a new command node (t_node_cmd),
 * and adds it to the list of commands. Finally, it returns the list of parsed commands.

static	t_node_cmd *parsing(t_list_n *list, t_minishell *minishell) // moved to parser/parse.c/parse_token
{
	t_node_cmd	*commands;
	t_node_cmd	*new;
	t_elem		*tmp;
	int			size;

	tmp = list->head;
	commands = NULL;
	while (tmp)
	{
		size = count_args(tmp);
		new = new_cmd_node(&tmp, size, mini);
		ft_add_cmd_end(&commands, new);
		if (tmp)
			tmp = tmp->next;
	}
	return (commands);
}
*/

/*
int is_full(char *str)  // removed - useless func
{
    int res;

    // Check if the path (specified by the string str) exists
	// Test for access to NAME using the real UID and real GID.
    res = access(str, F_OK);

    // If access returns 0, it means the path exists, so return TRUE
    if (res == 0)
        return TRUE;

    // If access returns a value other than 0, the path doesn't exist, so return FALSE
    return FALSE;
}
*/

/*
 *
 * The function takes a pointer to the head of the linked list (head), which consists of t_node_cmd nodes.
 * It initializes a pointer (current) to traverse the linked list, starting from the head.
 * Inside the loop, for each node, it checks if the command's path is already an absolute path using the is_full function (which is assumed to return a boolean value).
 * If the command's path is an absolute path, it allocates memory for the path attribute of the node and copies the absolute path using ft_strdup.
 * The return statement is used to exit the function after setting the path for the first node with an absolute path. Assuming that only one absolute path command is expected, this prevents unnecessary iterations.

static void abs_path_check(t_node_cmd **head) // moved to parser/parse.c/set_cmd_exec_path
{
    t_node_cmd *current;

    // Traverse the linked list of t_node_cmd nodes
    current = *head;
    while (current)
    {
        // Check if the command's path is already an absolute path
        if (is_full(current->cmd[0]))
        {
            // If it is, set the path attribute to the absolute path
            current->path = ft_strdup(current->cmd[0]);
            return; // Assuming that only one absolute path command is expected
        }

        // Move to the next node in the linked list
        current = current->next;
    }
}
*/

/*
 * handle the processing of a user input line in a shell-like program,
 * including reading input, checking syntax, and preparing data structures 
 * 
 * minishell->prompt: This is the prompt string that will be displayed to the user. It might look something like "username.minishell ~> ".
 * readline(minishell->prompt): This function reads a line of input from the user and displays the provided prompt.
 * It returns a dynamically allocated string containing the user's input.

int ft_line(t_minishell *minishell)  // moved to parser/parse.c/process_input_line
{
    // Read a line of input and assign it to minishell->line
    minishell->line = readline(minishell->prompt);

    // Check if the input line is NULL (e.g., when the user presses Ctrl+D for EOF)
    if (minishell->line == NULL)
    {
        // Clean up resources and exit the program
        ft_cleaning(mini);
        write(STDOUT_FILENO, "exit\n", 5);
        exit(0);
    }

    // Check if the input line is empty or contains only whitespaces
    if (ft_strlen(minishell->line) == 0 || ft_has_characters(minishell->line) == 0)
        return (1);

    // Add the input line to the history Place STRING at the end of the history list. The associated data field (if any) is set to NULL.
    add_history(minishell->line);  // from history.h

    // Tokenize the input line into a linked list of tokens (minishell->lex)
    minishell->lex = lexer(minishell->line);

    // Check the syntax of the input line
    if (sintax_check(minishell->lex))
    {
        minishell->exitstatus = 1;
        return (1);
    }

    // Count the number of pipes and commands in the input line
    minishell->nbr_pipes = get_pipe_nbr(minishell->lex);
    minishell->nbrofcom = minishell->nbr_pipes + 1;

    // Parse the input line into a linked list of commands (minishell->cmds)
    minishell->cmds = parsing(minishell->lex, mini);

    // Check for absolute paths in the commands
    return (abs_path_check(&minishell->cmds), 0);
}
 */


/*
 * This function essentially converts an array of environment variables into a linked list == from_envp

t_node *create_env_list(char **env)
{
    t_node *head;
    int i;

    i = 0;
    head = NULL;

    // Iterate through the array of environment variables
    while (env[i])
    {
        // Add each environment variable to the linked list
        ft_add_end(&head, env[i]);
        i++;
    }

    // Return the head of the linked list representing the environment variables
    return (head);
}

*/

/*
 * getenv("USER"): This function is used to retrieve the value of the environment variable with the name "USER." It returns a pointer to the value as a string.

void ft_minishell_init(t_minishell *minishell, char **envp)  // moved to main/minishell.c
{
    char	*tmp;

    // Initialize various fields of the t_minishell structure
    minishell->line = NULL;
    minishell->cmds = NULL;
    minishell->nbrofcom = 0;
    minishell->nbr_pipes = 0;
    minishell->fd_pipes = NULL;
    minishell->lex = NULL;

    // Create a linked list representing environment variables
    minishell->enviro = create_env_list(envp);

    // Set up the shell prompt
    tmp = getenv("USER");
    if (tmp != NULL)
        minishell->prompt = ft_strjoin(tmp, ".minishell ~> ");
    else
        minishell->prompt = ft_strjoin("beautiful", ".minishell ~> ");

    // Initialize exit status and old_cwd
    minishell->exitstatus = 0;
    tmp = getenv("PWD");
    if (tmp != NULL)
        minishell->old_cwd = ft_strdup(tmp);
    else
        minishell->old_cwd = ft_strdup(getenv("HOME"));

    // Initialize the home directory path
    minishell->home = ft_strdup(getenv("HOME"));

    // Fill the path array with executable search paths
    ft_path_filler(mini);
}
*/


/*
 *
 * The function begins by checking if the minishell->cmds field (presumably representing a list of commands) is not NULL. If it's not NULL, the remove_cmd_list function is called to free the memory associated with the command list.
 * Similarly, the function checks if the minishell->lex field (presumably representing a linked list of lexemes) is not NULL. If it's not NULL, the free_list function is called to free the memory associated with the lexeme list.
 * The function then resets the fields minishell->nbrofcom and minishell->nbr_pipes to zero, indicating no commands and no pipes.
 * The function checks if the minishell->fd_pipes field (presumably representing an array of file descriptors for pipes) is not NULL. If it's not NULL, the free function is called to free the memory associated with the array.
 * The function checks if the minishell->line field (presumably representing an input line) is not NULL. If it's not NULL, the free function is called to free the memory associated with the input line.
 * Finally, the function resets the pointers minishell->fd_pipes, minishell->lex, and minishell->cmds to NULL, indicating that these structures are now empty or uninitialized.

void ft_reset_values(t_minishell *minishell) // moved to main/minishell.c/minishell_reset
{
    // Remove the command list
    if (minishell->cmds)
        remove_cmd_list(&minishell->cmds);

    // Free the linked list representing the lexemes
    if (minishell->lex)
        free_list(minishell->lex);

    // Reset the number of commands and number of pipes
    minishell->nbrofcom = 0;
    minishell->nbr_pipes = 0;

    // Free the array storing file descriptors for pipes
    if (minishell->fd_pipes)
        free(minishell->fd_pipes);

    // Free the allocated memory for the input line
    if (minishell->line)
        free(minishell->line);

    // Reset pointers to NULL
    minishell->fd_pipes = NULL;
    minishell->lex = NULL;
    minishell->cmds = NULL;
}
*/

/*
 * using extern char **environ;
 * the code signals to the compiler that environ will be defined elsewhere 
 * (e.g., by the operating system or some runtime library), and the compiler should not allocate storage for it.
 * It is a global variable that holds a null-terminated array of strings,
 * where each string is in the form "name=value" representing an environment variable.
 * signal(SIGINT, ft_signal_handler); == signal handler for the SIGINT signal, which is typically generated when the user presses Ctrl+C 
 * When this signal is received, the function ft_signal_handler is called.
 * signal(SIGQUIT, SIG_IGN); == setting the action for the SIGQUIT signal to ignore (SIG_IGN)
 * SIGQUIT signal is generated by the terminal when the user presses Ctrl+\ 
*/
int	main(void)
{
	extern char	**environ;
	t_minishell		mini;
	/*
	
	*/
	signal(SIGINT, ft_signal_handler);
	signal(SIGQUIT, SIG_IGN);
	ft_minishell_init(&mini, environ);
	while (1)
	{
		if (ft_line(&mini) == 0)
			ft_executor(&mini, environ);
		ft_reset_values(&mini);
	}
	return (0);
}

