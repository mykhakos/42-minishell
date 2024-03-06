#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

int	ft_isnum(char *str);
char *ft_itoa(int n);
int	ft_strlen(char *str);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
char *ft_strndup(char *str, int n);

#endif // UTILS_H