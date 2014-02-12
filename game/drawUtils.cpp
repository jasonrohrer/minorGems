#include "drawUtils.h"

#include "minorGems/game/gameGraphics.h"

#include <math.h>
#include <string.h>


// pre-compute unit circle centered on 0,0 as a triangle fan
// num verts is 2 + num triangles
#define NUM_UNIT_CIRCLE_TRIS 16
#define NUM_UNIT_CRICLE_VERTS 18
static double unitCircleVerts[ NUM_UNIT_CRICLE_VERTS * 2 ];
static double workingCircleVerts[ NUM_UNIT_CRICLE_VERTS * 2 ];
// change for each unit circle drawn
static float unitCircleColors[ NUM_UNIT_CRICLE_VERTS * 4 ];


void initDrawUtils() {
    // center
    unitCircleVerts[0] = 0;
    unitCircleVerts[1] = 0;
    
    for( int v=1; v<NUM_UNIT_CRICLE_VERTS; v++ ) {
        double angle = 2 * M_PI * (v - 1.0)/(NUM_UNIT_CRICLE_VERTS - 2.0);
        
        unitCircleVerts[ v * 2 ] = cos( angle );
        unitCircleVerts[ v * 2 + 1 ] = sin( angle );
        }
    }


void freeDrawUtils() {
    }



void drawRect( double inStartX, double inStartY, 
               double inEndX, double inEndY ) {

    double vertices[8] = { inStartX, inStartY,
                           inStartX, inEndY,
                           inEndX, inEndY,
                           inEndX, inStartY };
    
    
            

    drawQuads( 1, vertices );
    }



void drawSquare( doublePair inCenter, double inRadius ) {
    drawRect( inCenter.x - inRadius, inCenter.y - inRadius,
              inCenter.x + inRadius, inCenter.y + inRadius );
    }



void drawRect( doublePair inCenter, 
               double inHorizontalRadius, double inVerticalRadius ) {
    
    drawRect( inCenter.x - inHorizontalRadius, 
              inCenter.y - inVerticalRadius,
              inCenter.x + inHorizontalRadius, 
              inCenter.y + inVerticalRadius );
    }  



void drawCircle( doublePair inCenter, double inRadius ) {

    memcpy( workingCircleVerts, unitCircleVerts,
            sizeof( double ) * NUM_UNIT_CRICLE_VERTS * 2 );
                    
    double cX = inCenter.x;
    double cY = inCenter.y;
    

    // scale it and move it
    for( int v=0; v<NUM_UNIT_CRICLE_VERTS; v++ ) {
        int i = v * 2;
                    
        workingCircleVerts[i] *= inRadius;
        workingCircleVerts[i] += cX; 
        
        workingCircleVerts[i+1] *= inRadius;
        workingCircleVerts[i+1] += cY;
        }
    
    // draw as fan
    drawTriangles( NUM_UNIT_CIRCLE_TRIS, workingCircleVerts,
                   false, true );
    }



void drawCircle( doublePair inCenter, double inRadius,
    Color inCenterColor, Color inOuterColor ) {

    memcpy( workingCircleVerts, unitCircleVerts,
            sizeof( double ) * NUM_UNIT_CRICLE_VERTS * 2 );
                    
    double cX = inCenter.x;
    double cY = inCenter.y;
    

    // scale it and move it
    for( int v=0; v<NUM_UNIT_CRICLE_VERTS; v++ ) {
        int i = v * 2;
                    
        workingCircleVerts[i] *= inRadius;
        workingCircleVerts[i] += cX; 
        
        workingCircleVerts[i+1] *= inRadius;
        workingCircleVerts[i+1] += cY;
      
        int c = v * 4;
                    
        // outside
        unitCircleColors[c++] = inOuterColor.r;
        unitCircleColors[c++] = inOuterColor.g;
        unitCircleColors[c++] = inOuterColor.b; 
        unitCircleColors[c++] = inOuterColor.a;
        }

    // inside 
    int c = 0;
    unitCircleColors[c++] = inCenterColor.r;
    unitCircleColors[c++] = inCenterColor.g;
    unitCircleColors[c++] = inCenterColor.b; 
    unitCircleColors[c++] = inCenterColor.a;
                
    // draw as fan
    drawTrianglesColor( NUM_UNIT_CIRCLE_TRIS, workingCircleVerts,
                        unitCircleColors, false, true );
    }

          

