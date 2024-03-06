#include "../include/lexer_parser.h"


    // if (str[i] == '<')
    // {
    //     if (str[i + 1] && str[i + 1] == '<')
    //         token_add_back(lst, new_token(str + i++, 2, HERE_DOC, state));
    //     else
    //         token_add_back(lst, new_token(str + i, 1, REDIR_IN, state));
    //     i++;
    // }
    // else if (str[i] == '>')
    // {
    //     if (str[i + 1] && str[i + 1] == '>')
    //         token_add_back(lst, new_token(str + i++, 2, DREDIR_OUT, state));
    //     else
    //         token_add_back(lst, new_token(str + i, 1, REDIR_OUT, state));
    //     i++;
    // }
    // return (i - idx);


/**
 * @brief Add a redirection token to the token list
 * 
 * @param lst The token list
 * @param str The string to store in the token
 * @param idx The index of the string stored in the token
 * @param state The state of the lexer
 * @return int The number of characters parsed
 */
static int tokenize_redir(t_token *lst, char *str, int idx, enum e_token_state state)
{
    int i;

	i = idx;
    if (ft_strncmp(str + i, TOKEN_HEREDOC, 2) == 0)
    {
        token_add_back(lst, new_token(TOKEN_HEREDOC, str + i, 2, state));
        return (2);
    }
    else if (ft_strncmp(str + i, TOKEN_REDIR_IN, 1) == 0)
    {
        token_add_back(lst, new_token(TOKEN_REDIR_IN, str + i, 1, state));
        return (1);
    }
    else if (ft_strncmp(str + i, TOKEN_DREDIR_OUT, 2) == 0)
    {
        token_add_back(lst, new_token(TOKEN_DREDIR_OUT, str + i, 2, state));
        return (2);
    }
    else if (ft_strncmp(str + i, TOKEN_REDIR_OUT, 1) == 0)
    {
        token_add_back(lst, new_token(TOKEN_REDIR_OUT, str + i, 1, state));
        return (1);
    }
    return (0);
}

/**
 * @brief Add an environment variable (e.g., $? or $0, $1, etc.) token to the token list
 * 
 * @param lst The token list
 * @param str The string to store in the token
 * @param state The state of the lexer
 * @return int The number of characters parsed
 */
static int tokenize_env_var(t_token *lst, char *str, enum e_token_state state)
{
    int i;

	i = 1;
    if (str[i] == '?' || (str[i] >= '0' && str[i] <= '9'))
        i++;
    else
    {
        while (is_alnum(str[i]) && str[i] != '\n' && str[i] != '\0')
            i++;
    }
	token_add_back(lst, new_token(str, i, ENV, state));
    return (i);
}

/**
 * @brief Add a quote token to the token list. This function handles the state
 * transitions between quotes (') and double quotes ("). It checks the current
 * lexer state and decides whether to start, end, or continue a quote state.
 * 
 * @param lst The token list
 * @param str The string to store in the token
 * @param state The state of the lexer
 * @param is_dquote A flag to indicate whether the quote is a double quote
 * @return 0
 */
static int tokenize_quotes(t_token *lst, char *str, enum e_token_state *state, int is_dquote)
{
    enum e_token_state e_state;
    enum e_token_type e_type;

    if (is_dquote)
    {
        e_state = IN_DQUOTE;
        e_type = DQUOTE;
    }
    else
    {
        e_state = IN_QUOTE;
        e_type = QOUTE;
    }
	if (*state == e_state)
	{
		token_add_back(lst, new_token(str, 1, e_type, GENERAL));
		*state = GENERAL;
	}
	else
	{
		token_add_back(lst, new_token(str, 1, e_type, *state));
		if (*state == GENERAL)
			*state = e_state;
	}
	return (0);
}

static int tokenize_word(t_token *lst, char *str, enum e_token_state *state)
{
    int i;

    i = 0;
    while (!is_printable(str[i]) || str[i] == ' ')
        i++;
	token_add_back(lst, new_token(str, i, WORD, *state));
    return (i);
}

/*
 * function effectively drives the tokenization process,
 * handling different cases based on the type of character encountered
 * and updating the linked list of tokens accordingly.
*/
int tokenize(t_token *tokens, char *str, int i, enum e_token_state *state)
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
            token_add_back(tokens, new_elem(str + i++, 1, WORD, *state));
        else
            i += get_env_token(tokens, str + i, *state);
    }
    // Check if the current character is a redirection symbol ('>' or '<')
    else if (str[i] == '>' || str[i] == '<')
        i += make_token_redir(tokens, str, i, *state);
    // Check if the current character is a pipe symbol ('|')
    else if (str[i] == '|')
        token_add_back(tokens, new_elem(str + i++, 1, PIPE_LINE, *state));
    // Check if the current character is a whitespace character
    else if (ft_isspace(str[i]))
        token_add_back(tokens, new_elem(str + i++, 1, WHITESPACE, *state));

    return i;
}
