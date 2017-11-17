/*
 * Modification History
 *
 * 2002-September-16    Jason Rohrer
 * Created.
 * Fixed a memory leak.
 *
 * 2002-September-25    Jason Rohrer
 * Added a setSetting function that takes a string value.
 *
 * 2002-September-26    Jason Rohrer
 * Added functions for integer values.
 *
 * 2003-January-11    Jason Rohrer
 * Added default values for float and int settings.
 *
 * 2003-August-24    Jason Rohrer
 * Fixed to remove 499-character limit for a setting value.
 *
 * 2009-February-12    Jason Rohrer
 * Added support for secure hashing.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2010-November-22    Jason Rohrer
 * More convenient getIntSetting call.
 *
 * 2011-January-3    Jason Rohrer
 * More convenient getFloatSetting call.
 *
 * 2017-April-13    Jason Rohrer
 * Functions for saving time settings in a future-proof format.
 *
 * 2017-November-1    Jason Rohrer
 * More convenient getStringSetting call (with default value).
 *
 * 2017-November-8    Jason Rohrer
 * getStringSetting now returns entire file contents.
 *
 * 2017-November-17    Jason Rohrer
 * getStringSetting back to returning first token, with new getSettingContents
 * function for getting entire file contents.
 */



#include "SettingsManager.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/Path.h"

#include "minorGems/crypto/hashes/sha1.h"



// will be destroyed automatically at program termination
SettingsManagerStaticMembers SettingsManager::mStaticMembers;

char SettingsManager::mHashingOn = false;



void SettingsManager::setDirectoryName( const char *inName ) {
    delete [] mStaticMembers.mDirectoryName;
    mStaticMembers.mDirectoryName = stringDuplicate( inName );
    }



char *SettingsManager::getDirectoryName() {
    return stringDuplicate( mStaticMembers.mDirectoryName );
    }



void SettingsManager::setHashSalt( const char *inSalt ) {
    delete [] mStaticMembers.mHashSalt;
    mStaticMembers.mHashSalt = stringDuplicate( inSalt );
    }



char *SettingsManager::getHashSalt() {
    return stringDuplicate( mStaticMembers.mHashSalt );
    }



void SettingsManager::setHashingOn( char inOn ) {
    mHashingOn = inOn;
    }




SimpleVector<char *> *SettingsManager::getSetting( 
    const char *inSettingName ) {

    char *fileContents = getSettingContents( inSettingName );
    
    if( fileContents == NULL ) {
        // return empty vector
        return new SimpleVector<char*>();
        }


    // else tokenize the file contents
    SimpleVector<char *> *returnVector = tokenizeString( fileContents );

    delete [] fileContents;
    
    return returnVector;
    }





char *SettingsManager::getSettingContents( const char *inSettingName ) {

    char *fileName = getSettingsFileName( inSettingName );
    File *settingsFile = new File( NULL, fileName );

    delete [] fileName;
    
    char *fileContents = settingsFile->readFileContents();

    delete settingsFile;

    
    if( fileContents == NULL ) {
        return NULL;
        }
    
    if( mHashingOn ) {
        
        char *hashFileName = getSettingsFileName( inSettingName, "hash" );
        
        File *hashFile = new File( NULL, hashFileName );
        
        delete [] hashFileName;
        
        char *savedHash = hashFile->readFileContents();
        
        delete hashFile;

        if( savedHash == NULL ) {
            printf( "Hash missing for setting %s\n", inSettingName );

            delete [] fileContents;
            return NULL;
            }
    
        
        // compute hash
        char *stringToHash = autoSprintf( "%s%s",
                                          fileContents,
                                          mStaticMembers.mHashSalt );

        char *hash = computeSHA1Digest( stringToHash );

        delete [] stringToHash;
        
        int difference = strcmp( hash, savedHash );
        
        delete [] hash;
        delete [] savedHash;
        

        if( difference != 0 ) {
            printf( "Hash mismatch for setting %s\n", inSettingName );
            
            delete [] fileContents;
            return NULL;
            }
        }
    
    return fileContents;
    }



char *SettingsManager::getSettingContents( const char *inSettingName,
                                           const char *inDefaultValue ) {
    char *val = getSettingContents( inSettingName );
    
    if( val == NULL ) {
        val = stringDuplicate( inDefaultValue );
        }

    return val;
    }



char *SettingsManager::getStringSetting( const char *inSettingName ) {
    char *value = NULL;
    
    SimpleVector<char *> *settingsVector = getSetting( inSettingName );

    int numStrings = settingsVector->size(); 
    if( numStrings >= 1 ) {

        char *firstString = *( settingsVector->getElement( 0 ) );

        value = stringDuplicate( firstString );
        }

    for( int i=0; i<numStrings; i++ ) {
        char *nextString = *( settingsVector->getElement( i ) );

        delete [] nextString;
        }
    
    delete settingsVector;

    return value;
    }



char *SettingsManager::getStringSetting( const char *inSettingName,
                                         const char *inDefaultValue ) {
    char *val = getStringSetting( inSettingName );
    
    if( val == NULL ) {
        val = stringDuplicate( inDefaultValue );
        }

    return val;
    }




float SettingsManager::getFloatSetting( const char *inSettingName,
                                        char *outValueFound ) {

    char valueFound = false;
    float value = 0;


    char *stringValue = getStringSetting( inSettingName );

    if( stringValue != NULL ) {

        int numRead = sscanf( stringValue, "%f",
                              &value );

        if( numRead == 1 ) {
            valueFound = true;
            }

        delete [] stringValue;
        }

    *outValueFound = valueFound;

    return value;
    }



float SettingsManager::getFloatSetting( const char *inSettingName,
                                        float inDefaultValue ) {
    char found;
    float value = getFloatSetting( inSettingName, &found );
    if( !found ) {
        value = inDefaultValue;
        }
    return value;
    }



int SettingsManager::getIntSetting( const char *inSettingName,
                                    char *outValueFound ) {

    char valueFound = false;
    int value = 0;


    char *stringValue = getStringSetting( inSettingName );

    if( stringValue != NULL ) {

        int numRead = sscanf( stringValue, "%d",
                              &value );

        if( numRead == 1 ) {
            valueFound = true;
            }

        delete [] stringValue;
        }

    *outValueFound = valueFound;

    return value;
    }



int SettingsManager::getIntSetting( const char *inSettingName,
                                    int inDefaultValue ) {
    char found;
    int value = getIntSetting( inSettingName, &found );
    if( !found ) {
        value = inDefaultValue;
        }
    return value;
    }





timeSec_t SettingsManager::getTimeSetting( const char *inSettingName,
                                           timeSec_t inDefaultValue ) {

    timeSec_t value = inDefaultValue;


    char *stringValue = getStringSetting( inSettingName );

    if( stringValue != NULL ) {
        
        sscanf( stringValue, "%lf", &value );
        
        delete [] stringValue;
        }

    return value;
    }




void SettingsManager::setSetting( const char *inSettingName,
                                  SimpleVector<char *> *inSettingVector ) {

    
    

    char **settingParts = inSettingVector->getElementArray();
    
    char *settingString = join( settingParts, inSettingVector->size(),
                                 "\n" );
    delete [] settingParts;
    
    setSetting( inSettingName, settingString );
    }



void SettingsManager::setSetting( const char *inSettingName,
                                  const char *inSettingValue ) {

    if( mHashingOn ) {
        
        // compute hash
        char *stringToHash = autoSprintf( "%s%s",
                                          inSettingValue,
                                          mStaticMembers.mHashSalt );

        char *hash = computeSHA1Digest( stringToHash );

        delete [] stringToHash;
        
        char *hashFileName = getSettingsFileName( inSettingName, "hash" );
    
        FILE *file = fopen( hashFileName, "w" );

        delete [] hashFileName;

        if( file != NULL ) {
            fprintf( file, "%s", hash );
            
            fclose( file );
            }
        
        delete [] hash;
        }
    



    FILE *file = getSettingsFile( inSettingName, "w" );
    
    if( file != NULL ) {
        
        fprintf( file, "%s", inSettingValue );
        
        fclose( file );
        }
    }



void SettingsManager::setSetting( const char *inSettingName,
                                  float inSettingValue ) {

    char *valueString = new char[ 15 ];
    sprintf( valueString, "%f", inSettingValue );

    setSetting( inSettingName, valueString );
    
    delete [] valueString;
    }



void SettingsManager::setSetting( const char *inSettingName,
                                  int inSettingValue ) {

    char *valueString = new char[ 15 ];
    sprintf( valueString, "%d", inSettingValue );

    setSetting( inSettingName, valueString );
    
    delete [] valueString;
    }



void SettingsManager::setSetting( const char *inSettingName,
                                  timeSec_t inSettingValue ) {

    // don't want a fixed buffer for printing doubles
    char *stringVal = autoSprintf( "%f", inSettingValue );
    
    setSetting( inSettingName, stringVal );
    
    delete [] stringVal;
    }




FILE *SettingsManager::getSettingsFile( const char *inSettingName,
                                        const char *inReadWriteFlags ) {
    char *fullFileName = getSettingsFileName( inSettingName );
    
    FILE *file = fopen( fullFileName, inReadWriteFlags );

    delete [] fullFileName;
    
    return file;
    }



char *SettingsManager::getSettingsFileName( const char *inSettingName ) {
    return getSettingsFileName( inSettingName, "ini" );
    }



char *SettingsManager::getSettingsFileName( const char *inSettingName,
                                            const char *inExtension ) {
    char **pathSteps = new char*[1];

    pathSteps[0] = mStaticMembers.mDirectoryName;

    char *fileName = new char[ strlen( inSettingName ) 
                               + strlen( inExtension )
                               + 2 ];

    sprintf( fileName, "%s.%s", inSettingName, inExtension );
    
    File *settingsFile = new File( new Path( pathSteps, 1, false ),
                                   fileName );

    delete [] fileName;

    // pathSteps copied internally by Path constructor
    delete [] pathSteps;
    

    char *fullFileName = settingsFile->getFullFileName();
    
    delete settingsFile;

    return fullFileName;
    }



SettingsManagerStaticMembers::SettingsManagerStaticMembers()
    : mDirectoryName( stringDuplicate( "settings" ) ),
      mHashSalt( stringDuplicate( "default_salt" ) ) {
    
    }



SettingsManagerStaticMembers::~SettingsManagerStaticMembers() {
    delete [] mDirectoryName;
    delete [] mHashSalt;
    }

