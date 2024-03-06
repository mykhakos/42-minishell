#ifndef ENV_H
# define ENV_H

#include <stdlib.h>

typedef struct s_env_var
{
    char *key;           // Represents the key of the environment variable
    char *value;         // Represents the value of the environment variable
    struct s_env_var *next; // Represents the next environment variable
} t_env_var;

char *extract_key(char *env);
char *extract_value(char *env);


#endif