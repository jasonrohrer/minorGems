/*
 * Modification History
 *
 * 2004-January-1   Jason Rohrer
 * Created.
 */



#include "NetworkFunctionLocks.h"


// instantiate static members
MutexLock NetworkFunctionLocks::mGetHostByNameLock;

MutexLock NetworkFunctionLocks::mInet_ntoaLock;

