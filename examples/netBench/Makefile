GXX=g++


DEBUG_ON_FLAG = -g
DEBUG_OFF_FLAG = 

DEBUG_FLAG = ${DEBUG_ON_FLAG}


# pthread library needed for linux
PLATFORM_FLAGS = -DLINUX -lpthread
# pthread library linked differently for BSD
# PLATFORM_FLAGS = -DBSD -pthread
# pthread flag not needed for OSX
#PLATFORM_FLAGS = -DBSD
# all sorts of other libraries needed for Solaris
#PLATFORM_FLAGS = -DSOLARIS -lsocket -lnsl -lresolv


COMPILE_FLAGS = ${DEBUG_FLAG} ${PLATFORM_FLAGS}

ROOT_PATH = ../../..

COMPILE_COMMAND = ${GXX} ${COMPILE_FLAGS} -I${ROOT_PATH}

OTHER_STUFF = ../../../minorGems/network/linux/SocketLinux.cpp ../../../minorGems/network/linux/SocketClientLinux.cpp ../../../minorGems/network/linux/SocketServerLinux.cpp ../../../minorGems/system/unix/TimeUnix.cpp



all:  netBenchSender netBenchReceiver

clean:
	rm *.o netBenchReceiver netBenchSender



netBenchSender: netBenchSender.o ${OTHER_STUFF}
	${COMPILE_COMMAND} -o netBenchSender netBenchSender.o ${OTHER_STUFF}

netBenchReceiver: netBenchReceiver.o ${OTHER_STUFF}
	${COMPILE_COMMAND} -o netBenchReceiver netBenchReceiver.o ${OTHER_STUFF}



netBenchSender.o:  netBenchSender.cpp
	${COMPILE_COMMAND} -c -o netBenchSender.o netBenchSender.cpp

netBenchReceiver.o:  netBenchReceiver.cpp
	${COMPILE_COMMAND} -c -o netBenchReceiver.o netBenchReceiver.cpp
