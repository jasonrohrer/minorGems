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

class A
{
public:
	~A() { delete (char*)123; }
};

int
main(int argc, char **argv)
{                   
	char *foo;
                   
    Fortify_EnterScope();               

	/* zero size test */
	foo =(char*) malloc(0);
	printf("malloc(0) %s\n", foo ? "succeeded" : "failed");

	/* zero size test */
	foo = new char[0];
	printf("new char[0] %s\n", foo ? "succeeded" : "failed");

                   
	foo = new char;

	/* note we use the incorrect deallocator */
	/* note this will only be detected if FORTIFY_PROVIDE_ARRAY_NEW
     * and FORTIFY_PROVIDE_ARRAY_DELETE are both turned on
     * (and your compiler supports them) */
	delete[] foo;
	*foo = 'Z';

	foo = new char;
	Fortify_LabelPointer(foo, "we use the wrong deallocator on this one");

	/* note we use the incorrect dealocator */
	free(foo);

	/* the destructor of this class does an illegal delete -
	 * demonstrates the delete-stack
	 */
	delete new A;

	Fortify_LeaveScope();

	return 1;
}






