#include "../include/env.h"


char *extract_key(char *env)
{
    int i;
    char *key;

    if (!env)
        return (NULL);
    i = 0;
    while (env[i] != '=')
        i++;
    key = ft_calloc(sizeof(char), (i + 1));
    if (key == NULL)
        return (NULL);
    i = 0;
    while (env[i] != '=')
    {
        key[i] = env[i];
        i++;
    }
    key[i] = '\0';
    return (key);
}

char *extract_value(char *env)
{
    int i;
    int j;
    char *value;

    if (!env)
        return (NULL);
    i = 0;
    while (env[i] != '=')
        i++;
    i++;
    j = i;
    while (env[j])
        j++;
    value = ft_calloc(sizeof(char), ((j - i) + 1));
    if (!value)
        return (NULL);
    j = 0;
    while (env[i])
    {
        value[j] = env[i];
        i++;
        j++;
    }
    value[j] = '\0';
    return (value);
}

t_env_var *env_var_new(char *key, char *value)
{
    t_env_var *new;

    new = (t_env_var *)malloc(sizeof(t_env_var));
    if (!new)
        return (NULL);
    new->key = key;
    new->value = value;
    new->next = NULL;
    return (new);
}

void env_var_add_back(t_env_var **lst, t_env_var *new)
{
    t_env_var *curr;

    if (!lst || !new)
        return ;
    if (!(*lst))
    {
        *lst = new;
        return (new);
    }
    curr = *lst;
    while (curr->next)
        curr = curr->next;
    curr->next = new;
}

t_env_var *env_var_from_envp(char **envp)
{
    t_env_var *lst;
    t_env_var *new;
    int i;
    char *key;
    char *value;

    lst = NULL;
    i = 0;
    while (envp[i])
    {
        key = extract_key(envp[i]);
        value = extract_value(envp[i]);
        new = env_var_new(key, value);
        if (!new)
            return (env_var_free(&lst));
        env_var_add_back(&lst, new);
        i++;
    }
    return (lst);
}

t_env_var *env_var_find(t_env_var *lst, char *key)
{
    t_env_var *curr;

    if (!lst || !key)
        return (NULL);
    curr = lst;
    while (curr)
    {
        if (ft_strcmp(curr->key, key) == 0)
            return (curr);
        curr = curr->next;
    }
    return (NULL);
}

void env_var_remove(t_env_var **lst, char *key)
{
    t_env_var *curr;
    t_env_var *prev;

    if (!lst || !(*lst) || !key)
        return ;
    curr = *lst;
    prev = NULL;
    while (curr)
    {
        if (ft_strcmp(curr->key, key) == 0)
        {
            if (prev)
                prev->next = curr->next;
            else
                *lst = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            break ;
        }
        prev = curr;
        curr = curr->next;
    }
}

t_env_var *env_var_free(t_env_var **lst)
{
    t_env_var *curr;
    t_env_var *next;

    if (!lst || !(*lst))
        return (NULL);
    curr = *lst;
    while (curr)
    {
        next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
    free(*lst);
    return (NULL);
}