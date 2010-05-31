#
# Modification History
#
# 2002-March-29    Jason Rohrer
# Added include path to make minorGems reachable.
# Added pthread linking and debug compile flag.
#


#
# GCC makefile for Fortify's test applications
# Disable -DFORTIFY to compile without Fortify
#
# To build both of the testers, it should simply be
# a matter of
#
#             make all
#
#

.SUFFIXES: .cxx .cpp

CC      = g++
CCFLAGS = -g -Wall -DFORTIFY -Wformat -pedantic -I../../../..
LN      = g++
LNFLAGS = -lpthread #-lm #-liostream

.c.o:
	@echo ---- $*.c ----
	@$(CC) -c $(CCFLAGS) $*.c

.cpp.o:
	@echo ---- $*.cpp ----
	@$(CC) -c $(CCFLAGS) $*.cpp

#.cpp.o:
#	@echo ---- $*.cpp ----
#	@$(CC) -c $(CCFLAGS) $*.cpp

all: test test2 test3





OBJS= test.o fortify.o
test: $(OBJS)
	@echo Linking test
	@$(LN) -o test $(OBJS) $(CCFLAGS) $(LNFLAGS)

OBJS2 = test2.o fortify.o
test2: $(OBJS2)
	@echo Linking test2
	@$(LN) -o test2 $(OBJS2) $(CCFLAGS) $(LNFLAGS)

OBJS3 = test3.o fortify.o
test3: $(OBJS3)
	@echo Linking test3
	@$(LN) -o test3 $(OBJS3) $(CCFLAGS) $(LNFLAGS)


clean: 
	rm -f test test2 test3 $(OBJS) $(OBJS2) $(OBJS3)



fortify.o: fortify.cpp fortify.h ufortify.h

test.o: test.c fortify.h ufortify.h

test2.o: test2.cpp fortify.h ufortify.h

test3.o: test3.cpp fortify.h ufortify.h
