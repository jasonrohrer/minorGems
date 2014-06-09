#ifndef GAME_STATE_INCLUDED
#define GAME_STATE_INCLUDED


#include "minorGems/util/SimpleVector.h"



// Interface for game states that can be fed into functions in minMax.h
class GameState {
    public:
        
        // relative state score for purpose of minMax search
        // (player 1 pushes score positively, player 2 negatively)
        virtual int getScore( char inDebug=false ) = 0;
        

        virtual char getGameOver() = 0;

        virtual SimpleVector<GameState *> getPossibleMoves() = 0;
        
        virtual ~GameState() {};


        virtual GameState *copy() = 0;
        
        virtual void copyFrom( GameState *inOther ) = 0;

        virtual void printState() = 0;
        
    };


#endif
