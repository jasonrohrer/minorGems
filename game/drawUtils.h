#include "minorGems/game/doublePair.h"
#include "minorGems/graphics/Color.h"


void initDrawUtils();
void freeDrawUtils();



void drawRect( double inStartX, double inStartY, 
               double inEndX, double inEndY );


void drawSquare( doublePair inCenter, double inRadius );

void drawRect( doublePair inCenter, 
               double inHorizontalRadius, double inVerticalRadius );


void drawCircle( doublePair inCenter, double inRadius );

void drawCircle( doublePair inCenter, double inRadius,
                 Color inCenterColor, Color inOuterColor );
