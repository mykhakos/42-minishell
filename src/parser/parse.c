#include "include/minishell.h"


static int count_args(t_token *token)
{
    int count;

    count = 0;
    while (token && token->type != PIPE_LINE)
    {
        if (token->type == WORD || token->type == ENV)
            count++;
        else if (token->type == DQUOTE || token->type == QOUTE)
        {
            token = token->next;
            while (token && token->type != DQUOTE && token->type != QOUTE)
                token = token->next;
            count++;
        }
        else if (is_redir(token->type))
        {
            while (token && token->type != WORD && token->type != ENV)
                token = token->next;
            token = token->next;
        }
        if (token)
            token = token->next;
    }
    return (count);
}

/*
 *
 * iterates through the input list, parsing tokens based on their types. 
 * It handles different token types, such as WORD, ENV, WHITE_SPACE, DOUBLE_QUOTE, QOUTE, and redirection tokens 
*/
static t_cmd *new_cmd_from_token(t_token **list, int size, t_minishell *minishell)
{
    t_cmd *cmd;
    int i;

    // Initialize a cmd t_cmd structure
    cmd = new_cmd_node_init(size, &i);
    if (cmd == NULL)
        return (NULL);

    // Loop through the list until a PIPE_LINE token is encountered
    while ((*list) && (*list)->type != PIPE_LINE)
    {
        // Check the type of the current token and perform parsing accordingly
        if ((*list)->type == WORD)
            parse_word(list, cmd->cmd_args, &i);
        else if ((*list)->type == ENV)
            parse_env(list, cmd->cmd_args, &i, minishell);
        else if ((*list)->type == WHITESPACE)
            (*list) = (*list)->next;
        else if ((*list)->type == DQUOTE || (*list)->type == QOUTE)
        {
            cmd->cmd_args[i] = parse_in_quotes(cmd->cmd_args[i], list, (*list)->type, minishell);
            if (cmd->cmd_args[i])
                i++;
        }
        else if (is_redir((*list)->type))
            parse_redir(list, minishell, cmd);
    }

    // Terminate the cmd array with NULL
    cmd->cmd_args[i] = NULL;
    return (cmd);
}


/*
 *
 * function takes a linked list of elements (t_list_n) and a data structure (t_minishell *minishell) as parameters.
 * It iterates through the elements of the linked list, counts the number of arguments in each command, creates a new command node (t_cmd),
 * and adds it to the list of commands. Finally, it returns the list of parsed commands.
*/
static	t_cmd *parse_token(t_token *token, t_minishell *minishell)
{
	t_cmd	*commands;
	t_cmd	*new;
	int		size;

	commands = NULL;
	while (token)
	{
		size = count_args(token);
		new = new_cmd_from_token(&token, size, minishell);
		cmd_add_back(&commands, new);
		token = token->next;
	}
	return (commands);
}

/*
 *
 * The function takes a pointer to the head of the linked list (head), which consists of t_cmd nodes.
 * It initializes a pointer (current) to traverse the linked list, starting from the head.
 * Inside the loop, for each node, it checks if the command's path is already an absolute path using the is_full function (which is assumed to return a boolean value).
 * If the command's path is an absolute path, it allocates memory for the path attribute of the node and copies the absolute path using ft_strdup.
 * The return statement is used to exit the function after setting the path for the first node with an absolute path. Assuming that only one absolute path command is expected, this prevents unnecessary iterations.
*/
static void set_cmd_exec_path(t_cmd *cmd)
{
    while (cmd)
    {
        if (access(cmd->cmd_args[0], F_OK) == 0)
        {
            cmd->exec_path = ft_strdup(cmd->cmd_args[0]);
            return ;
        }
        cmd = cmd->next;
    }
}

/*
 *
 * The get_pipe_nbr function takes a linked list of elements (t_list_n) and counts the number of pipe tokens (PIPE_LINE) in the list.
 * It iterates through the elements and increments the count 
*/
static int	get_pipe_nbr(t_token *token) // generalize this and move to token_utils
{
    int n;

	n = 0;
	while (token)
    {
        if (token->type == PIPE_LINE)
            n++;
        token = token->next;
    }
	return (n);
}

/*
 * handle the processing of a user input line in a shell-like program,
 * including reading input, checking syntax, and preparing data structures 
 * 
 * minishell->prompt: This is the prompt string that will be displayed to the user. It might look something like "username.minishell ~> ".
 * readline(minishell->prompt): This function reads a line of input from the user and displays the provided prompt.
 * It returns a dynamically allocated string containing the user's input.
 */
int process_input_line(t_minishell *minishell)  // this needs double pointer
{
    minishell->line = readline(minishell->prompt);
    if (minishell->line == NULL)
    {
        minishell_free(&minishell);  // remove & once minishell is double pointer
        write(STDOUT_FILENO, "exit\n", 5);
        exit(0);
    }
    if (ft_strlen(minishell->line) == 0 || ft_has_characters(minishell->line) == 0)
        return (1);
    add_history(minishell->line);  // from history.h
    minishell->lex = lexer(minishell->line);
    if (syntax_check(minishell->lex))
    {
        minishell->exitstatus = 1;
        return (1);
    }
    minishell->nbr_pipes = get_pipe_nbr(minishell->lex);
    minishell->nbrofcom = minishell->nbr_pipes + 1;
    minishell->cmds = parse_token(minishell->lex, minishell);
    set_cmd_exec_path(&minishell->cmds);
    return (0);
}
