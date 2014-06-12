
#include "minMax.h"


static MinOrMax switchSide( MinOrMax inSide ) {
    if( inSide == min ) {
        return max;
        }
    return min;
    }




static int betterScore( MinOrMax inSide, int inScoreA, int inScoreB ) {
    if( inSide == min ) {
        if( inScoreA < inScoreB ) {
            return inScoreA;
            }
        return inScoreB;
        }
    
    
    if( inScoreA > inScoreB ) {
        return inScoreA;
        }
    return inScoreB;
    }




static int leavesHit = 0;



GameState *minMaxPickMove( GameState *inCurrentState, MinOrMax inSide,
                           int inDepthLimit ) {
    
    leavesHit = 0;
    
    /*
    printf( "\n\nMinmax searching for move from:\n" );
    inCurrentState->printState();
    */

    SimpleVector<GameState *> possibleMoves = 
        inCurrentState->getPossibleMoves();
    

    if( possibleMoves.size() == 0 ) {
        // no moves left, game over?
        return NULL;
        }

    GameState *bestPossible = NULL;
    int bestScorePossible = INT_MIN;
    
    if( inSide == min ) {
        bestScorePossible = INT_MAX;
        }
    
    int numMoves = possibleMoves.size();

    for( int i=0; i<numMoves; i++ ) {
        
        GameState *move = *( possibleMoves.getElement( i ) );
    
        int score = minMax( move, switchSide( inSide ),
                            inDepthLimit );
        /*
        printf( "Looked at move with score [%d]:\n", score );
        move->printState();
        */
        
        bestScorePossible = betterScore( inSide, bestScorePossible, score );

        if( bestScorePossible == score ) {
            if( bestPossible != NULL ) {
                delete bestPossible;
                }
            
            bestPossible = move;
            }
        else {
            delete move;
            }
        }
    
    //printf( "\n%d leaves examined\n", leavesHit );

    return bestPossible;
    }






SimpleVector<GameState *> sortStatesByScore( SimpleVector<GameState*> inStates,
                                             MinOrMax inSide ) {
    
    int numStates = inStates.size();
    
    int highScore = INT_MIN;
    int lowScore = INT_MAX;
    
    // optimization:
    // if scores all identical, no sorting necessary
    
    for( int i=0; i<numStates; i++ ) {
        GameState *state = *( inStates.getElement(i) );    
        int score = state->getScore();    
        
        if( score > highScore ) {
            highScore = score;
            }
        if( score < lowScore ) {
            lowScore = score;
            }
        if( highScore != lowScore ) {
            break;
            }
        }
    if( highScore == lowScore ) {
        return inStates;
        }
    

    SimpleVector<int> scores;
    
    for( int i=0; i<numStates; i++ ) {
        GameState *state = *( inStates.getElement(i) );    
        scores.push_back( state->getScore() );
        }

    SimpleVector<GameState *> sortedStates;
    
    if( inSide == max ) {
        
        for( int i=0; i<numStates; i++ ) {
            
            int bestScore = INT_MIN;
            int bestIndex = -1;
            
            for( int j=0; j<inStates.size(); j++ ) {
                int score = *( scores.getElement( j ) );
                if( score  > bestScore ) {
                    bestScore = score;
                    bestIndex = j;
                    }
                }
            scores.deleteElement( bestIndex );
            
            sortedStates.push_back( *( inStates.getElement( bestIndex ) ) );
            inStates.deleteElement( bestIndex );                    
            }
        }
    if( inSide == min ) {
        
        for( int i=0; i<numStates; i++ ) {
            
            int bestScore = INT_MAX;
            
            int bestIndex = -1;
            
            for( int j=0; j<inStates.size(); j++ ) {
                int score = *( scores.getElement( j ) );
            
                if( score  < bestScore ) {
                    bestScore = score;
                    bestIndex = j;
                    }
                }
            scores.deleteElement( bestIndex );
            
            sortedStates.push_back( *( inStates.getElement( bestIndex ) ) );
            inStates.deleteElement( bestIndex );                    
            }
        }  

    return sortedStates;
    }





// followed pseudocode here:
// http://www.cs.cornell.edu/courses/cs312/2002sp/lectures/rec21.htm
int minMax( GameState *inCurrentState, MinOrMax inSide,
            int inDepthLimit, 
            int inMin,
            int inMax ) {
    
    if( inDepthLimit == 0 ) {
        leavesHit ++;
        return inCurrentState->getScore();
        }

    if( inCurrentState->getGameOver() ) {
        leavesHit ++;
        return inCurrentState->getScore();
        }

    
    SimpleVector<GameState *> possibleMoves = 
        inCurrentState->getPossibleMoves();
    
    int numMoves = possibleMoves.size();

    if( numMoves == 0 ) {
        // leaf
        leavesHit ++;
        return inCurrentState->getScore();
        }
    
    //possibleMoves = sortStatesByScore( possibleMoves, inSide );

    int bestScoreSeen;
        
    if( inSide == max ) {
        bestScoreSeen = inMin;
        
        for( int i=0; i<numMoves; i++ ) {
                
            GameState *move = *( possibleMoves.getElement( i ) );

        
            int score = minMax( move, switchSide( inSide ),
                                inDepthLimit - 1, bestScoreSeen, inMax );
            delete move;
            
            if( score > bestScoreSeen ) {
                bestScoreSeen = score;
                }
            if( bestScoreSeen > inMax ) {
                for( int j=i+1; j<numMoves; j++ ) {
                    delete *( possibleMoves.getElement( j ) );
                    }
                return inMax;
                }
            }
        }
    else {
        bestScoreSeen = inMax;

        for( int i=0; i<numMoves; i++ ) {
            
            GameState *move = *( possibleMoves.getElement( i ) );
            
            
            int score = minMax( move, switchSide( inSide ),
                                inDepthLimit - 1, inMin, bestScoreSeen );
            
            delete move;

            if( score < bestScoreSeen ) {
                bestScoreSeen = score;
                }
            if( bestScoreSeen < inMin ) {
                for( int j=i+1; j<numMoves; j++ ) {
                    delete *( possibleMoves.getElement( j ) );
                    }
                return inMin;
                }
            }

        }
    
    return bestScoreSeen;
    }


