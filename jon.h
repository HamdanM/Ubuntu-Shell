#include <stdlib.h>
#define	YES	1
#define	NO	0


char	*next_cmd(); //splitline.c,        jon
char	**splitline(char *); //splitline.c,           jon
void	freelist(char **); //splitline.c,            jon
void	*emalloc(size_t); //splitline.c,           jon
void	*erealloc(void *, size_t);   //splitline.c,          jon
void	fatal(char *, char *, int );