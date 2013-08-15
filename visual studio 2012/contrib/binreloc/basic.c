/* This file contains the basic BinReloc code. It is used to
 * generate more appropriate BinReloc source code, one which fits
 * your project's coding style and preffered API. Do not use this
 * file; run generate.pl instead.
 */

/*** LICENSE BEGIN */
/*
 * BinReloc - a library for creating relocatable executables
 * Written by: Hongli Lai <h.lai@chello.nl>
 * http://autopackage.org/
 *
 * This source code is public domain. You can relicense this code
 * under whatever license you want.
 *
 * See http://autopackage.org/docs/binreloc/ for
 * more information and how to use this.
 */
/*** LICENSE END */

/*** INCLUDE BEGIN */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
/*** INCLUDE END */

/*** ERROR BEGIN */
typedef enum {
	/* Cannot allocate memory. */
	BR_FIND_EXE_NOMEM,
	/* Unable to open /proc/self/maps; see errno for details. */
	BR_FIND_EXE_OPEN_MAPS,
	/* Unable to read from /proc/self/maps; see errno for details. */
	BR_FIND_EXE_READ_MAPS,
	/* The file format of /proc/self/maps is invalid; kernel bug? */
	BR_FIND_EXE_INVALID_MAPS,
	/* BinReloc is disabled. */
	BR_FIND_EXE_DISABLED
} BrFindExeError;
/*** ERROR END */


/*** FUNCTION BEGIN */
/*
 * Find the absolute filename of the executable. Returns the filename
 * (which must be freed) or NULL on error. If the parameter 'error' is
 * not NULL, the error code will be stored there.
 */
static char *
br_find_exe (BrFindExeError *error)
{
#ifdef BINRELOC_DISABLED
	if (error)
		*error = BR_FIND_EXE_DISABLED;
	return NULL;
#else
	char *path, *line, *result;
	size_t buf_size;
	ssize_t size;
	FILE *f;

	/* Read from /proc/self/exe (symlink) */
	if (sizeof (path) > SSIZE_MAX)
		buf_size = SSIZE_MAX - 1;
	else
		buf_size = PATH_MAX - 1;
	path = (char *) malloc (buf_size);
	if (path == NULL) {
		/* Cannot allocate memory. */
		if (error)
			*error = BR_FIND_EXE_NOMEM;
		return NULL;
	}

	size = readlink ("/proc/self/exe", path, buf_size);
	if (size != -1) {
		/* Success. */
		path[size] = '\0';
		return path;
	}


	/* readlink() failed; this can happen when the program is
	 * running in Valgrind 2.2. Read from /proc/self/maps as fallback. */

	buf_size = PATH_MAX + 128;
	line = (char *) realloc (path, buf_size);
	if (line == NULL) {
		/* Cannot allocate memory. */
		free (path);
		if (error)
			*error = BR_FIND_EXE_NOMEM;
		return NULL;
	}

	f = fopen ("/proc/self/maps", "r");
	if (f == NULL) {
		free (line);
		if (error)
			*error = BR_FIND_EXE_OPEN_MAPS;
		return NULL;
	}

	/* The first entry should be the executable name. */
	result = fgets (line, (int) buf_size, f);
	if (result == NULL) {
		fclose (f);
		free (line);
		if (error)
			*error = BR_FIND_EXE_READ_MAPS;
		return NULL;
	}

	/* Get rid of newline character. */
	buf_size = strlen (line);
	if (buf_size <= 0) {
		/* Huh? An empty string? */
		fclose (f);
		free (line);
		if (error)
			*error = BR_FIND_EXE_INVALID_MAPS;
		return NULL;
	}
	if (line[buf_size - 1] == 10)
		line[buf_size - 1] = 0;

	/* Extract the filename; it is always an absolute path. */
	path = strchr (line, '/');

	/* Sanity check. */
	if (strstr (line, " r-xp ") == NULL || path == NULL) {
		fclose (f);
		free (line);
		if (error)
			*error = BR_FIND_EXE_INVALID_MAPS;
		return NULL;
	}

	path = strdup (path);
	free (line);
	fclose (f);
	return path;
#endif /* BINRELOC_DISABLED */
}
/*** FUNCTION END */
