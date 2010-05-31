#include <stdio.h>
#include <stdlib.h>
#include "fortify.h"

/*
 * NOTE: The Fortify routines will compile away to nothing
 * if FORTIFY isn't defined in the makefile.
 *
 * DO NOT insert #define FORTIFY here. It Will Not Work.
 * All files (including fortify.cxx) need FORTIFY to be
 * defined. The correct place for this is the makefile.
 */


int
main(int argc, char **argv)
{                   
	char *foo, *bar;
   
    Fortify_EnterScope();

	/* note that we never free this memory */
	foo = malloc(123);

	Fortify_LabelPointer(foo, "we never free this memory!");

	/* note that we read this memory after it's been freed */	
	foo = malloc(124);
	*foo = 'X';
	free(foo);
	printf("Should be X: '%c'\n", *foo);

	/* note we've already freed this memory */	
	free(foo);
	
	/* note we write to memory that's been realloc'd, using the old pointer */
	foo = malloc(125);
	bar = realloc(foo, 126);
	*foo = 'X';
	free(bar);	
	
	/* note we write before the block */
	foo = malloc(127);
	Fortify_LabelPointer(foo, "we write before this block!");
	*(foo-1) = 'Z';
	free(foo);
	
	/* note we write after the block */
	bar = "I'm going to eat you little fishie!";
	foo = malloc(strlen(bar));
	strcpy(foo, bar);
	free(foo);

	/* we never allocated this memory */
	free(bar);	

	Fortify_LeaveScope();
	Fortify_OutputStatistics(); 

	return 42;
}
