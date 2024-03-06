# include "unistd.h"

char *ft_itoa(int n)
{
    char	*str;
    int		len;
    int		sign;
    int		tmp;

    sign = (n < 0);
    len = 1 + sign;
    tmp = n;
    while (tmp /= 10)
        len++;
    str = (char *)malloc(sizeof(char) * (len + 1));
    if (!str)
        return (NULL);
    str[len] = '\0';
    if (sign)
        str[0] = '-';
    while (len-- > sign)
    {
        str[len] = (n % 10) * (1 - 2 * sign) + '0';
        n /= 10;
    }
    return (str);
}
