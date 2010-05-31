#
# Modification History
#
# 2004-January-16    Jason Rohrer
# Switched to use minorGems platform-independed mutexes.
#


CC = g++

# Source files
SRC := LeakTracer.cc

ROOT_PATH = ../../../..

# Switch comment to select a platform
# PLATFORM_MUTEX = $(ROOT_PATH)/minorGems/system/win32/MutexLockWin32.cpp
PLATFORM_MUTEX = $(ROOT_PATH)/minorGems/system/linux/MutexLockLinux.cpp -pthread

# Comment both of these out to disable thread safetly
C_THREAD=-DTHREAD_SAVE -D_REENTRANT -D_THREAD_SAFE
O_THREAD = $(PLATFORM_MUTEX)


# Common flags
C_FLAGS = -g -pipe -Wall -W -I$(ROOT_PATH) $(C_THREAD)
O_FLAGS = $(C_FLAGS) $(O_THREAD)

# Object files
OBJ_DIR = .
OBJ   := $(patsubst %.cc,$(OBJ_DIR)/%.o,$(SRC))
SHOBJ := $(patsubst %.o,$(OBJ_DIR)/%.so,$(OBJ))

.PHONY: all clean tidy distrib test

all: $(OBJ) $(SHOBJ)

clean:	tidy 
	rm -f $(OBJ) leak.out

tidy:
	rm -f *~ *orig *bak *rej

tags:	$(SRC) $(INCL)
	ctags $(SRC) $(INCL)

distrib: clean all README.html
	(cd .. && tar cvfz  /root/drylock/LeakTracer/LeakTracer.tar.gz --exclude LeakTracer/CVS --exclude LeakTracer/old --exclude LeakTracer/test LeakTracer/)

$(OBJ_DIR)/%.o: %.cc
	$(CC) -fPIC -c $(C_FLAGS) $< -o $@

$(OBJ_DIR)/%.so : $(OBJ_DIR)/%.o
	$(CC) $(O_FLAGS) -shared -o $@ $<

README.html: README
	/root/ed/mcl/util/htmlize.pl README

test:
	$(CC) $(C_FLAGS) test.cc -o test
	./test
	./LeakCheck ./test
	./leak-analyze ./test
#	./compare-test test.template test.result
