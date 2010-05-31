#include "PNGImageConverter.h"

#include "minorGems/graphics/Image.h"

#include "minorGems/io/file/FileOutputStream.h"

#include "minorGems/system/Time.h"


int main() {

    int imageSize = 640;
    Image testImage( imageSize, imageSize, 3, false );

    
    // red fades toward bottom
    // green fades toward right
    double *red = testImage.getChannel( 0 );
    double *green = testImage.getChannel( 1 );

    for( int y=0; y<imageSize; y++ ) {
        for( int x=0; x<imageSize; x++ ) {
            red[y*imageSize+x] = 1.0 - ( y / (double)imageSize );
            green[y*imageSize+x] = 1.0 - ( x / (double)imageSize );
            green[y*imageSize+x] = 0;
            }
        }
    
    PNGImageConverter png;


    File outFileB( NULL, "test.png" );
    FileOutputStream outStreamB( &outFileB );


    double t = Time::getCurrentTime();
                    
    png.formatImage( &testImage, &outStreamB );

    printf( "Converter took %f seconds\n", 
            Time::getCurrentTime() - t );

    return 0;
    }
