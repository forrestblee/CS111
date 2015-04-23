// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"


#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

// general call for all types of commands   
void callCommand(command_t c);

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
		if (inp_close == -1)
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
void executeANDCommand(command_t c)
{
	callCommand(c->u.command[0]);
	int cStatus1 = command_status(c->u.command[0]);
	if (cStatus1) 	// if true (fails, returns -1), whole AND command fails, you lose, get a life
		c->status = cStatus1;
	else {		// false, first command succeeded, run second command and save status in AND command
		callCommand(c->u.command[1]);
		c->status = command_status(c->u.command[1]);
	}
}

void executeSequenceCommand(command_t c)
{
	callCommand(c->u.command[0]);
	callCommand(c->u.command[1]);
	c->status = command_status(c->u.command[0]);
}
void executeORCommand(command_t c)
{
	callCommand(c->u.command[0]);
	int cStatus1 = command_status(c->u.command[0]);
	if (cStatus1) 	// if true (fails, returns -1), try the other command
	{
		callCommand(c->u.command[1]);
		c->status = command_status(c->u.command[1]);
	}	
	else {		// false, first command succeeded, can forget about the second command
		c->status = cStatus1;
	}
}
	
void executeSimpleCommand(command_t c)
{
	pid_t p = fork();
	//if child process is running, wait till finish, store status
	if (p > 0)
	{
		int status;
		waitpid(p, &status, 0);
		if (!WIFEXITED(status))
			error(3, 0, "runtime error: child process returned non-exit status");
		c->status = status;
	}
	else if (p == 0)
	{
		setupIO(c);
		execvp(c->u.word[0], c->u.word);
		// if execvp returns at all, error
		error (3, 0, "runtime error: %s command failed", c->u.word[0]);
	}
	else
		error(3, 0, "runtime error: forking failed");
}

void executeSubshellCommand(command_t c)
{
	callCommand(c->u.subshell_command);
	c->status = command_status(c->u.subshell_command);
}

// general call for all types of commands   
void callCommand(command_t c)
{
	enum command_type type = c->type;
	switch (type)
	{
		case AND_COMMAND:
			executeANDCommand(c);
			break;
		case SEQUENCE_COMMAND:   
			executeSequenceCommand(c);
			break;
		case OR_COMMAND:
			executeORCommand(c);
			break;
		case PIPE_COMMAND:
			break;
		case SIMPLE_COMMAND:
			executeSimpleCommand(c);
			break;
		case SUBSHELL_COMMAND:
			executeSubshellCommand(c);
			break;
		default:
		// should never get here
			error(3, 0, "runtime error: command not recognized");
			break;
	}
}

void
execute_command (command_t c, bool time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
	callCommand(c);
	
//  error (1, 0, "command execution not yet implemented");
}
