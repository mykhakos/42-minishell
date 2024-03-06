#include "../include/env_var.h"


char *extract_env_var_key(char *var)
{
    int i;
    char *key;

    if (!var)
        return (NULL);
    i = 0;
    while (var[i] != '=')
        i++;
    key = ft_calloc(sizeof(char), (i + 1));
    if (key == NULL)
        return (NULL);
    i = 0;
    while (var[i] != '=')
    {
        key[i] = var[i];
        i++;
    }
    key[i] = '\0';
    return (key);
}

char *extract_env_var_value(char *var)
{
    int i;
    int j;
    char *value;

    if (!var)
        return (NULL);
    i = 0;
    while (var[i] != '=')
        i++;
    i++;
    j = i;
    while (var[j])
        j++;
    value = ft_calloc(sizeof(char), ((j - i) + 1));
    if (!value)
        return (NULL);
    j = 0;
    while (var[i])
    {
        value[j] = var[i];
        i++;
        j++;
    }
    value[j] = '\0';
    return (value);
}
