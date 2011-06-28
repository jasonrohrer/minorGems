#include "jri.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"
#include "minorGems/graphics/RGBAImage.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"


int main( int inNumArgs, char**inArgs  ) {
    

    if( inNumArgs != 3 ) {
		printf( "Usage:  jriToTga  in.jri out.tga\n" );
		return 1;
		}


    File f( NULL, inArgs[1] );
    
    int jriSize;
    unsigned char *jriBytes = f.readFileContents( &jriSize );

    if( jriBytes != NULL ) {
        int w, h;
        
        rgbaColor *rgba = extractJRI( jriBytes, jriSize, &w, &h );
        
        if( rgba != NULL ) {
            
            Image image( w, h, 3, false );
            

            int numPixels = w * h;
            
            double *channels[3];
            
            for( int c=0; c<3; c++ ) {
                channels[c] = image.getChannel( c );
                }
            
            for( int p=0; p<numPixels; p++ ) {
                rgbaColor color = rgba[p];
                
                channels[0][p] = color.r / 255.0;
                channels[1][p] = color.g / 255.0;
                channels[2][p] = color.b / 255.0;
                }
            
            File f2( NULL, inArgs[2] );
            
            FileOutputStream fOut( &f2 );
            TGAImageConverter converter;
            

            converter.formatImage( &image, &fOut );

            delete [] rgba;
            }
        else {
            printf( "Parsing JRI data from file '%s' failed\n", inArgs[1] );
            }

        delete [] jriBytes;
        }
    else {
        printf( "Reading from file '%s' failed\n", inArgs[1] );
        }
    }

