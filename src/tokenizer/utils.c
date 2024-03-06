#include "../include/tokenizer.h"

int	is_space(char c)
{
	return (
		c == ' ' || c == '\t' || c == '\v'
		|| c == '\r' || c == '\f' || c == '\n'
		);
}

int is_num(char c)
{
    return (c >= '0' && c <= '9');
}

int is_alpha(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

