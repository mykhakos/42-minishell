#include <stdlib.h>

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
    while (n > 0 && *s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
        n--;
    }
    if (n == 0)
        return (0);
    return (*s1 - *s2);
}
