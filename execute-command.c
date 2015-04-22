// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"


#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcnl.h>


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void setupIO(command_t c)
{
	if (c->input != NULL)
	{
		//create input file descriptor, can be read, possibly written
		int inp_fd = open(c->input, O_RDWR);
		if (inp_fd == -1)
			error(2, 0, "io error: unable to read %s", c->input);
		//attempt to copy file description
		int inp_duplicate = dup2(inp_fd, 0);
		if (inp_duplicate == -1)
			error(2, 0, "io error: dup2 failed");
		//attempt to close input stream
		int inp_close = close(inp_fd);
		if (int_close == -1)
			error(2, 0, "io error: unable to close input");
	}
	if (c->output != NULL)
	{
		//create output file descriptor, can be read, written
		// possibly needs to be created or overwritten
		int	out_fd = open(c->output, O_WRONLY, O_CREAT, O_TRUNC);
		if (out_fd == -1)
			error(2, 0, "io error: unable to read %s", c->output);
		//attempt to copy file description
		int out_duplicate = dup2(out_fd, 0);
		if (out_duplicate == -1)
			error(2, 0, "io error: dup2 failed");
		//attempt to close input stream
		int out_close = close(out_fd);
		if (out_close == -1)
			error(2, 0, "io error: unable to close output file");
	}
}
void executeSimpleCommand(command_t c)
{
	pid_t p = fork();
	//if child process is running, wait till finish, store status
	if (p > 0)
	{
		// for now we assume the child command succeeds
		int status;
		waitpid(p, &status, 0);
		if (!WIFEXITED(status))
			error(3, 0, "runtime error: child process returned non-exit status");
		c->status = status;
	}
	else if (p == 0)
	{
		setupIO(c);
		
	}
}
void
execute_command (command_t c, bool time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
	 
//  error (1, 0, "command execution not yet implemented");
}
