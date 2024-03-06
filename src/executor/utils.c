#include "../include/executor.h"

/*
 *
 * function checks whether each file descriptor (fd1 and fd2) is greater than 0, indicating that it is a valid file descriptor. 
 * If so, it closes the file descriptor using the close system call and updates the value to -2 or -3, depending on whether it's the first or second file descriptor.
 * The use of these specific values (-2 and -3) after closing the file descriptors might be indicative of some special meaning or state in the context of the program's design.
 * It's common in Unix-like systems to use negative values to represent certain states or flags.
 * 
*/
void	ft_close_two_fd(int *fd1, int *fd2)
{
	// If fd1 is a valid file descriptor, close it and update to -2
	if (*fd1 > 0)
	{
		close(*fd1);
		*fd1 = -2;
	}

	// If fd2 is a valid file descriptor, close it and update to -3
	if (*fd2 > 0)
	{
		close(*fd2);
		*fd2 = -3;
	}
}