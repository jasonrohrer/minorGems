#ifndef DOUBLE_PAIR_INCLUDED
#define DOUBLE_PAIR_INCLUDED


typedef struct doublePair {
        double x;
        double y;
    } doublePair;


doublePair add( doublePair inP, double inV );

doublePair add( doublePair inA, doublePair inB );

doublePair sub( doublePair inA, doublePair inB );



double distance( doublePair inA, doublePair inB );


// angle relative to (0,0)
double angle( doublePair inA );


char equal( doublePair inA, doublePair inB );


#endif
