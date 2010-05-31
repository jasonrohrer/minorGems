/*
 * Modification History
 *
 * 2002-August-2   Jason Rohrer
 * Created.
 * Fixed a buffer size error.
 *
 * 2002-September-17   Jason Rohrer
 * Changed to use the SettingsManager.
 *
 * 2002-November-9   Jason Rohrer
 * Added support for matching address patterns.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */



#include "ConnectionPermissionHandler.h"



#include "minorGems/io/file/File.h"

#include "minorGems/util/SettingsManager.h"



ConnectionPermissionHandler::ConnectionPermissionHandler() {

    SimpleVector<char *> *addressVector =
        SettingsManager::getSetting( "allowedWebHosts" );

    int numAddresses = addressVector->size();

    mPermittedAddresses = new SimpleVector<HostAddress *>();
    mPermittedPatterns = new SimpleVector<char *>();
    
    for( int i=0; i<numAddresses; i++ ) {

        char *addressString = *( addressVector->getElement( i ) );

        char *starLocation = strstr( addressString, "*" );
        
        if( starLocation == NULL ) {
            // an address
        
            mPermittedAddresses->push_back(
                new HostAddress( addressString, 0 ) );
            }
        else {
            // an address pattern
            mPermittedPatterns->push_back( addressString );
            }
        }

    delete addressVector;
    }



ConnectionPermissionHandler::~ConnectionPermissionHandler() {
    int numAddresses = mPermittedAddresses->size();
    int i;
    
    for( i=0; i<numAddresses; i++ ) {

        HostAddress *address = *( mPermittedAddresses->getElement( i ) );

        delete address;        
        }

    int numPatterns = mPermittedPatterns->size();
    
    for( i=0; i<numPatterns; i++ ) {
        
        char *pattern = *( mPermittedPatterns->getElement( i ) );

        delete [] pattern;        
        }
    
    delete mPermittedAddresses;
    delete mPermittedPatterns;
    }


char ConnectionPermissionHandler::isPermitted( HostAddress *inAddress ) {
    
    int numAddresses = mPermittedAddresses->size();
    int i;
    for( i=0; i<numAddresses; i++ ) {

        HostAddress *address = *( mPermittedAddresses->getElement( i ) );

        if( address->equals( inAddress ) ) {
            return true;
            }
        }

    // didn't match any address exactly

    
    // check if it matches one of our patterns
    HostAddress* numericalAddress = inAddress->getNumericalAddress();
    if( numericalAddress == NULL ) {
        return false;
        }

    char *addressString = numericalAddress->mAddressString;

    int numPatterns = mPermittedPatterns->size();

    char foundMatch = false;
    
    for( i=0; i<numPatterns && !foundMatch; i++ ) {

        char *pattern = *( mPermittedPatterns->getElement( i ) );
        
        int patternLength = strlen( pattern );

        int addressLength = strlen( addressString );

        char done = false;
        
        for( int j=0; j<patternLength && j<addressLength && !done; j++ ) {

            if( pattern[j] == '*' ) {
                foundMatch = true;
                done = true;
                }
            else if( pattern[j] != addressString[j] ) {
                done = true;
                foundMatch = false;
                }            
            }
        }

    delete numericalAddress;
    
    return foundMatch;
    }


