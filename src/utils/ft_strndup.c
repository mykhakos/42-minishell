#include "include/minishell.h"

char *ft_strndup(char *str, int n)
{
    size_t str_len;
    char *copy;
    int i;

    i = 0;
    str_len = ft_strlen(str);
    copy = malloc(sizeof(char) * (str_len + 1));
    if (!copy)
        return (NULL);
    while (str[i] && i < n)
    {
        copy[i] = str[i];
        i++;
    }
    copy[i] = '\0';
    return (copy);
}
