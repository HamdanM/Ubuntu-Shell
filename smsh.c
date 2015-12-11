#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	<string.h>
#include	"idris.h"		// execute programs (built-in and created ones) and support control flow
#include	"jon.h"			// Run built in commands, support local variables, and contains tokenizer
#include	"tom.h"			// Process User commands and a simple storage system to store name=value pairs with facility to mark items as part of the environment


/**
 **		Idris, Jon, and Tom shell
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 ***/

#define	DFL_PROMPT	"> "

int main() {
	char	*cmdline, *prompt, **arglist;
	int	result, process(char **);
	void	setup();
	prompt = DFL_PROMPT ;
	setup();
	while ( (cmdline = next_cmd(prompt, stdin)) != NULL ) {		
		// idris extra credit
		history(cmdline);
		if ( (arglist = splitline(cmdline)) != NULL  ) {
			result = process(arglist);
			freelist(arglist);
		}
		free(cmdline);
	}
	return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
	extern char **environ;

	VLenviron2table(environ);
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}
