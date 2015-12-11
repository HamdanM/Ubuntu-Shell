#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	<string.h>
#include	"idris.h"
#include	"jon.h"
#include	"tom.h"

#define		LIMIT 256

char *historyVal[LIMIT];
int historySize = 0;

void history(char *Value) {
	historyVal[historySize] = malloc(LIMIT);
	strcpy(historyVal[historySize], Value);
	historySize++;
	return;
}

int is_history_command(char *s) {
	return (strcmp(s,"history")==0);
}

void do_history_command() {
	int i = 0;
	for(i = 0; i < historySize; i++) {
		printf("%d  %s\n", (i+1), historyVal[i]);
	}
	return;
}


// executes functions

/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 *  errors: -1 on fork() or wait() errors
 */
int execute(char *argv[]) {
	extern char **environ;
	int	pid ;
	int	child_info = -1;

	if (strcmp(argv[0], "cd") == 0) {
		if (argv[1] != NULL) {
			chdir(argv[1]);
			return 0;
		}
	} else if (strcmp(argv[0], "exit") == 0) {
		exit(1);
	}

	if ( argv[0] == NULL )		/* nothing succeeds	*/
		return 0;

	if ( (pid = fork())  == -1 )
		perror("fork");
	else if ( pid == 0 ){		
		/**if(strcmp(argv[1], ">") == 0) {
			FILE *fd = fopen(argv[2], "W");
			dup2(fileno(fd), fileno(stdout));
			execvp(argv[0], argv);
			exit(1);
		}*/
			environ = VLtable2environ();
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			execvp(argv[0], argv);
			perror("cannot execute command");
			exit(1);
		
	}
	else {
		if ( wait(&child_info) == -1 )
			perror("wait");
	}
	return child_info;
}




// Control Flow only for if/then/else/fi and if/then/fi

enum states   { NEUTRAL, WANT_THEN, THEN_BLOCK , ELSE_BLOCK};
enum results  { SUCCESS, FAIL };

static int if_state  = NEUTRAL;
static int if_result = SUCCESS;
static int last_stat = 0;

int	syn_err(char *);

/*
 * purpose: determine the shell should execute a command
 * returns: 1 for yes, 0 for no
 * details: if in THEN_BLOCK and if_result was SUCCESS then yes
 *          if in THEN_BLOCK and if_result was FAIL    then no
 *          if in WANT_THEN  then syntax error (sh is different)
 */
int ok_to_execute() {
	int	rv = 1;		/* default is positive */

	if ( if_state == WANT_THEN ) {
		syn_err("then expected");
		rv = 0;
	} else if ( if_state == THEN_BLOCK && if_result == SUCCESS ) {
		rv = 1;
	} else if ( if_state == THEN_BLOCK && if_result == FAIL ) {
		rv = 0;
	} else if ( if_state == ELSE_BLOCK && if_result == SUCCESS ) {
		rv = 0;
	} else if ( if_state == ELSE_BLOCK && if_result == FAIL ) {
		rv = 1;
	}
	return rv;
}

/*
 * purpose: boolean to report if the command is a shell control command
 * returns: 0 or 1
 */
int is_control_command(char *s) {
	return (strcmp(s,"if")==0 || strcmp(s,"then")==0 || strcmp(s,"else")==0 || strcmp(s,"fi")==0);
}

/*
 * purpose: Process "if", "then", "fi" - change state or detect error
 * returns: 0 if ok, -1 for syntax error
 *   notes: I would have put returns all over the place, Barry says "no"
 */
int do_control_command(char **args) {
	char	*cmd = args[0];
	int	rv = -1;

	if( strcmp(cmd,"if")==0 ){
		if ( if_state != NEUTRAL )
			rv = syn_err("if unexpected");
		else {
			last_stat = process(args+1);
			if_result = (last_stat == 0 ? SUCCESS : FAIL );
			if_state = WANT_THEN;
			rv = 0;
		}
	}
	else if ( (strcmp(cmd,"then")==0) ){
		if ( if_state != WANT_THEN )
			rv = syn_err("then unexpected");
		else {
			if_state = THEN_BLOCK;
			rv = 0;
		}
	}
	else if ( (strcmp(cmd,"else")==0)){
		if ( if_state != THEN_BLOCK ) {
			rv = syn_err("else unexpected"); 
		} else {
			if_state = ELSE_BLOCK;
			rv = 0;
		}
	}
	else if ( strcmp(cmd,"fi")==0 ){
		if (( if_state != THEN_BLOCK ) && ( if_state != ELSE_BLOCK )) { //&& ( if_state != THEN_BLOCK )) {
			rv = syn_err("fi unexpected"); 
		} else {
			if_state = NEUTRAL;
			rv = 0;
		}
	}
	else 
		fatal("internal error processing: ", cmd, 2);
	return rv;
}

/* purpose: handles syntax errors in control structures
 * details: resets state to NEUTRAL
 * returns: -1 in interactive mode. Should call fatal in scripts
 */
int syn_err(char *msg) {
	if_state = NEUTRAL;
	fprintf(stderr,"syntax error: %s\n", msg);
	return -1;
}
