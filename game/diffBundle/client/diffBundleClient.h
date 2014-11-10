


char isUpdatePlatformSupported();



// returns true if update process started, or false if
// auto-update not available
char startUpdate( char *inUpdateServerURL, int inOldVersionNumber );

// take anoter non-blocking step
// return 1 if request complete
// return -1 if request hit an error
// return 0 if request still in-progress
int stepUpdate();


// return fraction of update completion from 0 to 1
float getUpdateProgress();


// frees resources associated with an update
// if update is not complete, this cancels it (possibly in a partial state)
// if hostname lookup is not complete, this call might block.
void clearUpdate();
