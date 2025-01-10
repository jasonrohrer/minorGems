#include "RandomSource.h"

#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"

#include "minorGems/graphics/Image.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"


void genRandomImage( RandomSource *inSource ) {
    int w = 64;
    int h = 500;
    
    Image im( w, h, 3, false );
    
    double *channels[3];
    for( int c=0; c<3; c++ ) {
        channels[c] = im.getChannel( c );
        }
    
    int numPixels = w * h;
    
    for( int i=0; i<numPixels; i++ ) {
        
        for( int c=0; c<3; c++ ) {
            double v = inSource->getRandomDouble();
            channels[c][i] = v;
            }
        }
    
    File outFile( NULL, "randomImage.tga" );
    
    FileOutputStream outStream( &outFile );
    
    TGAImageConverter conv;
    
    conv.formatImage( &im, &outStream );
    }




#include "CustomRandomSource.h"
#include "StdRandomSource.h"
#include "JenkinsRandomSource.h"
#include "XoshiroRandomSource.h"

int main() {
    
    //JenkinsRandomSource randSource( 11234258 );
    //CustomRandomSource randSource( 11234258 );
    //StdRandomSource randSource( 11 );
    XoshiroRandomSource randSource( 0 );
    
    genRandomImage( &randSource );
    

    return 0;
    }
