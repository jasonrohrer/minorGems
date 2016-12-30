#ifndef DOUBLE_PAIR_INCLUDED
#define DOUBLE_PAIR_INCLUDED


typedef struct doublePair {
        double x;
        double y;
    } doublePair;


doublePair add( doublePair inP, double inV );

doublePair add( doublePair inA, doublePair inB );

doublePair sub( doublePair inA, doublePair inB );

doublePair mult( doublePair inP, double inV );


doublePair normalize( doublePair inP );

double length( doublePair inP );


// same direction, new length
doublePair forceLength( doublePair inP, double inLength );




double distance( doublePair inA, doublePair inB );


// angle relative to (1,0)
double angle( doublePair inA );


char equal( doublePair inA, doublePair inB );


doublePair rotate( doublePair inP, double inAngle );


double dot( doublePair inA, doublePair inB );



#endif
