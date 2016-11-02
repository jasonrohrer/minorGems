/*
 * Modification History
 *
 * 2002-February-25    Jason Rohrer
 * Created.  
 *
 * 2011-February-16    Jason Rohrer
 * Flag to print next log message to std out.
 */



#include "Log.h"



const int Log::DEACTIVATE_LEVEL = 0;

const int Log::CRITICAL_ERROR_LEVEL = 1;

const int Log::ERROR_LEVEL = 2;

const int Log::WARNING_LEVEL = 3;

const int Log::INFO_LEVEL = 4;

const int Log::DETAIL_LEVEL = 5;

const int Log::TRACE_LEVEL = 6;



Log::Log()
        : mPrintOutNextMessage( false ),
          mPrintAllMessages( false ) {

    }



Log::~Log() {

    }



void Log::printOutNextMessage() {
    mPrintOutNextMessage = true;
    }



void Log::printAllMessages( char inPrintAlso ) {
    mPrintAllMessages = inPrintAlso;
    }


