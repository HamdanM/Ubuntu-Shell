#include <stdlib.h>
#define	YES	1
#define	NO	0

int is_control_command(char *);
int do_control_command(char **);
int ok_to_execute();
int builtin_command(char **, int *);
int	process();    //process2.c

int	VLenviron2table(char **);       //varlib.h
int	VLexport(char *);			//varlib.h
char	*VLlookup(char *);		//varlib.h
void	VLlist();				//varlib.h
int	VLstore( char *, char * );	//varlib.h
char	**VLtable2environ();	//varlib.h

void	fatal(char *, char *, int );