#include <stdio.h>
#include <stdlib.h>
#include "normal-binreloc.h"

int
main ()
{
	char *exe;
	BrFindExeError error;

	exe = br_find_exe (&error);
	if (exe == NULL) {
		printf ("Error %d\n", (int) error);
		return 1;
	}
	printf ("%s\n", exe);
	free (exe);
	return 0;
}
