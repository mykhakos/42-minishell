#include "../include/executor.h"


void clear_input_on_interrupt(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}

void exit_on_interrupt(int sig)
{
	if (sig == SIGINT)
		exit(130);
}

