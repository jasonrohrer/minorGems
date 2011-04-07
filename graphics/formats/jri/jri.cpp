#include "jri.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"


#define JRI_VERSION  1


rgbaColor *extractJRI( unsigned char *inData, int inNumBytes,
                       int *outWidth, int *outHeight ) {
    
    int version, w, h, numColors;
    

    int numRead =
        sscanf( (char *)inData, "%d %d %d %d", &version, &w, &h, &numColors );
    
    
    if( numRead != 4 ) {
        return NULL;
        }
    
    if( version != JRI_VERSION ) {
        return NULL;
        }
    
    //printf( "JRI data for %dx%d image with %d colors\n", w, h, numColors );


    // skip header by looking for '#'

    int index = 0;

    while( index < inNumBytes ) {    
        if( inData[index++] == '#' ) {
            break;
            }
        }

    //printf( "Index of first data byte = %d\n", index );
    
    if( index >= inNumBytes ) {
        return NULL;
        }
    

    int numDataBytes = inNumBytes - index;
    
    inData = &( inData[ index ] );
    

    if( numDataBytes < 3 * numColors ) {
        // not enough data for palette
        return NULL;
        }
    
    rgbaColor *colors = new rgbaColor[ numColors ];
    

    int b = 0;
    
    for( int c=0; c<numColors; c++ ) {
        colors[c].r = inData[b++];
        colors[c].g = inData[b++];
        colors[c].b = inData[b++];
        colors[c].a = 255;

        //printf( "Color %d,%d,%d\n", colors[c].r, colors[c].g, colors[c].b );
        }

    // skip palette
    numDataBytes = numDataBytes - b;
    inData = &( inData[ b ] );

    
    int numPixels = w * h;
    

    rgbaColor *pixels = new rgbaColor[ numPixels ];
    int p = 0;

    
    b = 0;
    while( b < numDataBytes ) {
    
        // a new run or non-run

        int runType = inData[b++];
        int runLength = inData[b++];
        

        if( runType == 1 ) {
            // run

            int runByte = inData[b++];

            //printf( "Length %d run of byte %d\n", runLength, runByte );

            for( int r=0; r<runLength; r++ ) {
                pixels[p++] = colors[ runByte ];
                }
            }
        else {
            // non-run
            
            //printf( "Length %d non-run\n", runLength );

            for( int r=0; r<runLength; r++ ) {
                pixels[p++] = colors[ inData[b++] ];
                }
            }
        }
    


    delete [] colors;
    
        

    *outWidth = w;
    *outHeight = h;
    
    return pixels;
    }




static void outputNonRunBytes( SimpleVector<unsigned char> *dataVector,
                               SimpleVector<unsigned char> *nonRunBytes ) {
    
    int numNonRun = nonRunBytes->size();
            
    if( numNonRun > 0 ) {
        
        dataVector->push_back( 0 );
        dataVector->push_back( (unsigned char)numNonRun );
        
        //printf( "Outputting %d-length non-run\n", numNonRun );
        
        for( int b=0; b<numNonRun; b++ ) {
            
            dataVector->push_back( 
                *( nonRunBytes->getElement( b ) ) );
            }
        nonRunBytes->deleteAll();
        }
    }






unsigned char *generateJRI( rgbaColor *inRGBA, int inWidth, int inHeight,
                            int *outNumBytes ) {

    int numPixels = inWidth * inHeight;
        

    SimpleVector<rgbaColor> colors;
    char colorOverflow = false;
    
    unsigned char *pixelIndices = new unsigned char[ numPixels ];
    
    // build palette and indices into palette for each pixel
    for( int p=0; p<numPixels; p++ ) {
        unsigned char r, g, b;
        
        r = inRGBA[p].r;
        g = inRGBA[p].g;
        b = inRGBA[p].b;

        int foundIndex = -1;

        int numColors = colors.size();
        for( int c=0; c<numColors; c++ ) {
            rgbaColor color = *( colors.getElement( c ) );

            if( color.r == r && 
                color.g == g && 
                color.b == b ) {
                foundIndex = c;
                
                break;
                }
            }

        if( foundIndex != -1 ) {
            pixelIndices[p] = (unsigned char)foundIndex;
            }
        else {
            // new color
            if( colors.size() < 256 ) {
                colors.push_back( inRGBA[p] );
                
                pixelIndices[p] = (unsigned char)( colors.size() - 1 );
                }
            else {
                // palette is full
                pixelIndices[p] = 0;
                colorOverflow = true;
                }
            }
        }        

    if( colorOverflow ) {
        delete [] pixelIndices;
        return NULL;
        }

    int numColors = colors.size();

    //printf( "JRI found %d colors\n", numColors );
    

    char *header = autoSprintf( "%d\n%d %d\n%d\n",
                                JRI_VERSION, 
                                inWidth, inHeight, 
                                colors.size() );
    

    SimpleVector<unsigned char> dataVector;
    

    dataVector.push_back( (unsigned char*)header, (int)strlen( header ) );
    
    delete [] header;

    dataVector.push_back( '#' );

    // add palette bytes
    for( int c=0; c<numColors; c++ ) {
        rgbaColor color = *( colors.getElement( c ) );
        
        dataVector.push_back( color.r );
        dataVector.push_back( color.g );
        dataVector.push_back( color.b );
        }
    
    
    
    // run-length encode pixel index bytes

    SimpleVector<unsigned char> nonRunBytes;
    
    unsigned char currentRunByte = 0;
    
    unsigned char currentRunLength = 0;
    
    
    for( int p=0; p<numPixels; p++ ) {
        if( currentRunLength == 0 ) {
            currentRunByte = pixelIndices[p];
            currentRunLength ++;
            }
        else if( currentRunByte == pixelIndices[p] ) {
            if( currentRunLength < 255 ) {
                currentRunLength ++;
                }
            else {
                // run full, output it

                // first, output and clear any non-run bytes that preceded 
                // the run
                outputNonRunBytes( &dataVector, &nonRunBytes );


                dataVector.push_back( 1 );
                dataVector.push_back( currentRunLength );
                dataVector.push_back( currentRunByte );

                //printf( "Outputting %d-length run of byte %d\n", 
                //        currentRunLength, currentRunByte );

                // start a fresh run
                currentRunLength = 1;
                // keep current byte
                }
            }
        else if( currentRunLength < 3 ) {
            // a non-run that seemed like a run

            // add this non-run to our non-run byte vector
            for( int r=0; r<currentRunLength; r++ ) {
                
                if( nonRunBytes.size() == 255 ) {
                    // a full non-run
                    // output it
                    
                    outputNonRunBytes( &dataVector, &nonRunBytes );
                    }
                
                nonRunBytes.push_back( currentRunByte );
                }

            // see if a fresh run is starting
            currentRunByte = pixelIndices[p];
            currentRunLength = 1;
            }
        else {
            // current run of length 3 or greater
            // AND current run broken


            // first, output and clear any non-run bytes that preceded the run
            outputNonRunBytes( &dataVector, &nonRunBytes );
            
        
            // now output the run

            dataVector.push_back( 1 );
            dataVector.push_back( currentRunLength );
            dataVector.push_back( currentRunByte );
            
            //printf( "Outputting %d-length run of byte %d\n", 
            //        currentRunLength, currentRunByte );

            // start a fresh run with this new, run-breaking byte
            currentRunLength = 1;
            currentRunByte = pixelIndices[p];
            }
        }
    

    // final runs?
    outputNonRunBytes( &dataVector, &nonRunBytes );
    
    if( currentRunLength > 0 ) {
        dataVector.push_back( 1 );
        dataVector.push_back( currentRunLength );
        dataVector.push_back( currentRunByte );
        
        //printf( "Outputting %d-length run of byte %d\n", 
        //        currentRunLength, currentRunByte );
        }
    

    
    delete [] pixelIndices;

            
    *outNumBytes = dataVector.size();
        
    return dataVector.getElementArray();
    }

