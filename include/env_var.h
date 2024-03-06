#ifndef ENV_H
# define ENV_H

#include <stdlib.h>

typedef struct s_env_var
{
    char *key;
    char *value;
    struct s_env_var *next;
} t_env_var;


t_env_var *new_env_var(char *key, char *value);
void env_var_add_back(t_env_var **lst, t_env_var *new);
t_env_var *env_var_from_envp(char **envp);  // this should be a static function somewhere
t_env_var *env_var_find(t_env_var *lst, char *key);
void env_var_remove(t_env_var **lst, char *key);
void env_var_free(t_env_var **lst);

char *extract_env_var_key(char *var);
char *extract_env_var_value(char *var);

#endif