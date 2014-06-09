
#include "GameState.h"

#include <limits.h>


enum MinOrMax {
	min,
    max };



GameState *minMaxPickMove( GameState *inCurrentState, MinOrMax inSide,
                           int inDepthLimit = -1 );



// performs minMax search from inCurrentState and returns
// minMax score for inCurrentState
//
// inSide is the player, Min or Max, that would be making a move
// next from this game state.
//
// inMin and inMax are used internally, in recursive self-calls,
// for alpha-beta pruning
int minMax( GameState *inCurrentState, MinOrMax inSide,
            int inDepthLimit = -1, 
            int inMin = INT_MIN,
            int inMax = INT_MAX );

