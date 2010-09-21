#include "doublePair.h"

#include <math.h>


doublePair add( doublePair inP, double inV ) {
    inP.x += inV;
    inP.y += inV;
    return inP;
    }


doublePair add( doublePair inA, doublePair inB ) {
    inA.x += inB.x;
    inA.y += inB.y;
    
    return inA;
    }


doublePair sub( doublePair inA, doublePair inB ) {
    inA.x -= inB.x;
    inA.y -= inB.y;
    
    return inA;
    }



double distance( doublePair inA, doublePair inB ) {
    double delX = inA.x - inB.x;
    double delY = inA.y - inB.y;
    
    return sqrt( delX * delX + delY * delY );
    }



double angle( doublePair inA ) {
    double x = inA.x;
    double y = inA.y;

    double angle = atan2( y, x );
    
    if( angle < 0 ) {
        angle += 2 * M_PI;
        }
    return angle;
    }



char equal( doublePair inA, doublePair inB ) {
    return ( inA.x == inB.x ) && ( inA.y == inB.y );
    }




