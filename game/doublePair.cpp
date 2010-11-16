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


doublePair mult( doublePair inP, double inV ) {
    inP.x *= inV;
    inP.y *= inV;
    return inP;
    }


static doublePair defaultNormalized = { 0, 1 };


doublePair normalize( doublePair inP ) {
    double length = sqrt( inP.x * inP.x + inP.y * inP.y );
    
    if( length == 0 ) {
        return defaultNormalized;
        }
    
    double factor = 1 / length;
    inP.x *= factor;
    inP.y *= factor;
    return inP;
    }


doublePair forceLength( doublePair inP, double inLength ) {
    return mult( normalize( inP ), inLength );
    }


double length( doublePair inP ) {
    return sqrt( inP.x * inP.x + inP.y * inP.y );
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


doublePair rotate( doublePair inP, double inAngle ) {
    double cosTheta = cos( inAngle );
    double sinTheta = sin( inAngle );
    
    doublePair result = 
        { cosTheta * inP.x - sinTheta * inP.y,
          sinTheta * inP.x + cosTheta * inP.y };

    return result;
    }



double dot( doublePair inA, doublePair inB ) {
    return inA.x * inB.x + inA.y * inB.y;
    }




