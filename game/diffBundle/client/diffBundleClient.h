


char isUpdatePlatformSupported();



// returns true if update process started, or false if
// auto-update not available
char startUpdate( char *inUpdateServerURL, int inOldVersionNumber );

// take anoter non-blocking step
// return 1 if request complete
// return -1 if request hit an error
// return 0 if request still in-progress
int stepUpdate();

// if stepUpdate returns -1, this returns true if the error
// was a write error (likely due to file permissions)
char wasUpdateWriteError();



// return fraction of update completion from 0 to 1
float getUpdateProgress();


// frees resources associated with an update
// if update is not complete, this cancels it (possibly in a partial state)
// if hostname lookup is not complete, this call might block.
void clearUpdate();



// should be called from updated client after it loads
// does post-update cleanup that cannot be done from old exe.
//
// Should be called at the end of launch to allow any old exe that forked
// a new exe to quit and get out of the way.
//
// Note that this is safe to call at launch even if no update was done.
void postUpdate();
