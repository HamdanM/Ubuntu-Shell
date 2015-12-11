#include <stdlib.h>
#define	YES	1
#define	NO	0

int	execute(char **);

int ok_to_execute();
int is_control_command(char *);
int do_control_command(char **);
int syn_err(char *);

void history(char *);
int is_history_command(char *);
void do_history_command();

void	fatal(char *, char *, int );
