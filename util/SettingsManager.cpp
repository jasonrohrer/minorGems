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

    char *fileName = getSettingsFileName( inSettingName );
    File *settingsFile = new File( NULL, fileName );

    delete [] fileName;
    
    char *fileContents = settingsFile->readFileContents();

    delete settingsFile;


    
    if( fileContents == NULL ) {
        // return empty vector
        return new SimpleVector<char *>();
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
            return new SimpleVector<char *>();
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
            return new SimpleVector<char *>();
            }
        }


    // else tokenize the file contents
    SimpleVector<char *> *returnVector = tokenizeString( fileContents );

    delete [] fileContents;
    
    return returnVector;
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



void SettingsManager::setSetting( const char *inSettingName,
                                  SimpleVector<char *> *inSettingVector ) {

    
    

    char **settingParts = inSettingVector->getElementArray();
    
    char *settingString = join( settingParts, inSettingVector->size(),
                                 "\n" );
    delete [] settingParts;
    

    if( mHashingOn ) {
        
        // compute hash
        char *stringToHash = autoSprintf( "%s%s",
                                          settingString,
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
        
        fprintf( file, "%s", settingString );
        
        fclose( file );
        }

    delete [] settingString;

    // else do nothing
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
                                  const char *inSettingValue ) {
    SimpleVector<char *> *settingsVector = new SimpleVector<char *>( 1 );

    settingsVector->push_back( (char *)inSettingValue );

    setSetting( inSettingName, settingsVector );

    delete settingsVector; 
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

