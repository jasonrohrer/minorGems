#include "jri.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"
#include "minorGems/graphics/RGBAImage.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"


int main( int inNumArgs, char**inArgs  ) {
    

    if( inNumArgs != 3 ) {
		printf( "Usage:  tgaToJri  in.tga out.jri\n" );
		return 1;
		}


    File f( NULL, inArgs[1] );
    
    FileInputStream fIn( &f );
    

    TGAImageConverter converter;
    

    Image *image = converter.deformatImage( &fIn );	
    
    if( image != NULL ) {
        
        unsigned char *rgbaBytes = RGBAImage::getRGBABytes( image );

        int jriSize;
        
        unsigned char *jriBytes = 
            generateJRI( (rgbaColor*)rgbaBytes, 
                         image->getWidth(), image->getHeight(),
                         &jriSize );
        
        delete [] rgbaBytes;

        if( jriBytes != NULL ) {
            FILE *outFile = fopen( inArgs[2], "wb" );
            

            if( outFile != NULL ) {
                
                int numWritten = fwrite( jriBytes, 1, jriSize, outFile );

                if( numWritten != jriSize ) {
                    printf( "Writing %d bytes to file '%s' failed\n",
                            jriSize, inArgs[2] );
                    }

                fclose( outFile );
                }
            else {
                printf( "Opening file '%s' for writing failed\n",
                        inArgs[2] );
                }

            delete [] jriBytes;
            }
        else {
            printf( "Converting image to JRI failed.  "
                    "More than 256 colors?\n" );
            }
        
        

        delete image;
        }
    else {
        printf( "Reading TGA from file '%s' failed\n", inArgs[1] );
        }
    
    }

