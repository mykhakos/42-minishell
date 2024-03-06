#include "include/minishell.h"

/*
 *
 * env command printing the environment variables stored in the linked list.
 * The function checks if the linked list (minishell->enviro) is not empty before proceeding.
 * It iterates through the linked list, skipping the printing of specific environment variables such as "LINES" and "COLUMNS."
 * For each node in the linked list, it prints the key, equal sign, and value of the environment variable.
 * The continue statement is used to skip the printing of specific variables and move to the next iteration of the loop.
*/
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
