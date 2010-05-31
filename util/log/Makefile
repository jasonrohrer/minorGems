#
# Modification History
#
# 2002-February-25   Jason Rohrer
# Created.
# Changed to be more succinct.
#


GXX=g++ 

ROOT_PATH = ../../..

DEBUG_ON_FLAG = -g
DEBUG_OFF_FLAG = 

DEBUG_FLAG = ${DEBUG_OFF_FLAG}

TIME_PLATFORM_PATH = unix
TIME_PLATFORM = Unix



TIME_O = ${ROOT_PATH}/minorGems/system/${TIME_PLATFORM_PATH}/Time${TIME_PLATFORM}.o
TIME_H = ${ROOT_PATH}/minorGems/system/Time.h
TIME_CPP = ${ROOT_PATH}/minorGems/system/${TIME_PLATFORM_PATH}/Time${TIME_PLATFORM}.cpp




testLog: AppLog.o FileLog.o PrintLog.o Log.o testLog.o ${TIME_O}
	${GXX} ${DEBUG_FLAG} -o testLog AppLog.o FileLog.o PrintLog.o Log.o testLog.o ${TIME_O}



clean:
	rm -f *.o ${TIME_O}


testLog.o: testLog.cpp AppLog.h FileLog.h Log.h
	${GXX} ${DEBUG_FLAG} -o testLog.o -c testLog.cpp

AppLog.o: AppLog.h AppLog.cpp Log.h PrintLog.h
	${GXX} ${DEBUG_FLAG} -o AppLog.o -c AppLog.cpp

PrintLog.o: PrintLog.h PrintLog.cpp Log.h PrintLog.h ${TIME_H}
	${GXX} ${DEBUG_FLAG} -I${ROOT_PATH} -o PrintLog.o -c PrintLog.cpp

FileLog.o: PrintLog.h FileLog.cpp FileLog.h Log.h 
	${GXX} ${DEBUG_FLAG} -o FileLog.o -c FileLog.cpp

Log.o: Log.h Log.cpp
	${GXX} ${DEBUG_FLAG} -o Log.o -c Log.cpp

${TIME_O}: ${TIME_H} ${TIME_CPP}
	${GXX} ${DEBUG_FLAG} -I${ROOT_PATH} -o ${TIME_O} -c ${TIME_CPP}





