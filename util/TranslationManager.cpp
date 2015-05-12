/*
 * Modification History
 *
 * 2004-October-7    Jason Rohrer
 * Created.
 *
 * 2006-February-19    Jason Rohrer
 * Fixed an inconsistency in memory management.
 *
 * 2008-September-17    Jason Rohrer
 * Support for setting language data directly.
 *
 * 2010-March-21    Jason Rohrer
 * Fixed crash if default language file not found.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 *
 * 2015-May-12    Jason Rohrer
 * Support for alternate languages that add keys to a language.
 */

#include "TranslationManager.h"

#include <stdio.h>

#include "minorGems/io/file/File.h"



// will be destroyed automatically at program termination
TranslationManagerStaticMembers TranslationManager::mStaticMembers;



void TranslationManager::setDirectoryName( const char *inName ) {
    mStaticMembers.setDirectoryAndLanguage( inName,
                                            mStaticMembers.mLanguageName,
                                            true );
    }



char *TranslationManager::getDirectoryName() {
    return stringDuplicate( mStaticMembers.mDirectoryName );
    }



char **TranslationManager::getAvailableLanguages( int *outNumLanguages ) {
    File *languageDirectory = new File( NULL, mStaticMembers.mDirectoryName );

    if( languageDirectory->exists() && languageDirectory->isDirectory() ) {

        int numChildFiles;
        File **childFiles = languageDirectory->getChildFiles( &numChildFiles );
                
        if( childFiles != NULL ) {
            SimpleVector<char*> *languageNames = new SimpleVector<char*>();

            for( int i=0; i<numChildFiles; i++ ) {

                char *name = childFiles[i]->getFileName();

                char *extensionPointer = strstr( name, ".txt" );

                if( extensionPointer != NULL ) {
                    // terminate string, cutting off extension
                    extensionPointer[0] = '\0';

                    languageNames->push_back( stringDuplicate( name ) );
                    }

                delete [] name;
                delete childFiles[i];
                }
            delete [] childFiles;

            char **returnArray = languageNames->getElementArray();

            *outNumLanguages = languageNames->size();

            delete languageNames;


            delete languageDirectory;
            return returnArray;
            }

        }

    delete languageDirectory;
    
    // default, if we didn't return above
    *outNumLanguages = 0;
    return new char*[0];
    }



void TranslationManager::setLanguage( const char *inLanguageName,
                                      char inClearOldKeys ) {
    mStaticMembers.setDirectoryAndLanguage( mStaticMembers.mDirectoryName,
                                            inLanguageName,
                                            inClearOldKeys );
    }



void TranslationManager::setLanguageData( const char *inData,
                                          char inClearOldKeys ) {
    mStaticMembers.setTranslationData( inData, inClearOldKeys );
    }



const char *TranslationManager::translate( const char *inTranslationKey ) {

    char *translatedString = NULL;

    SimpleVector<char *> *keys =
        mStaticMembers.mTranslationKeys;

    SimpleVector<char *> *naturalLanguageStrings =
        mStaticMembers.mNaturalLanguageStrings;

    
    if( keys != NULL ) {
        int numKeys = keys->size();

        for( int i=0; i<numKeys && translatedString == NULL; i++ ) {

            if( strcmp( inTranslationKey, *( keys->getElement( i ) ) ) == 0 ) {
                // keys match
                translatedString =
                    *( naturalLanguageStrings->getElement( i ) );
                }
            }
        }

    
    if( translatedString == NULL ) {
        // no translation exists

        // the translation for this key is the key itself

        // add it to our translation table

        char *key = stringDuplicate( inTranslationKey );
        char *value = stringDuplicate( inTranslationKey );

        keys->push_back( key );
        naturalLanguageStrings->push_back( value );

        // thus, we return a value from our table, just as if a translation
        // had existed for this string
        translatedString = value;
        }

    return translatedString;
    }


        
TranslationManagerStaticMembers::TranslationManagerStaticMembers()
    : mDirectoryName( NULL ),
      mLanguageName( NULL ),
      mTranslationKeys( NULL ),
      mNaturalLanguageStrings( NULL ) {

    // default
    setDirectoryAndLanguage( "languages", "English", true );
    }



TranslationManagerStaticMembers::~TranslationManagerStaticMembers() {
    if( mDirectoryName != NULL ) {
        delete [] mDirectoryName;
        }
    
    if( mLanguageName != NULL ) {
        delete [] mLanguageName;
        }

    if( mTranslationKeys != NULL ) {
        int numKeys = mTranslationKeys->size();

        for( int i=0; i<numKeys; i++ ) {
            delete [] *( mTranslationKeys->getElement( i ) );
            }
        delete mTranslationKeys;
        mTranslationKeys = NULL;
        }

    if( mNaturalLanguageStrings != NULL ) {
        int numKeys = mNaturalLanguageStrings->size();

        for( int i=0; i<numKeys; i++ ) {
            delete [] *( mNaturalLanguageStrings->getElement( i ) );
            }
        delete mNaturalLanguageStrings;
        mNaturalLanguageStrings = NULL;
        }

    }


        
void TranslationManagerStaticMembers::setDirectoryAndLanguage(
    const char *inDirectoryName,
    const char *inLanguageName,
    char inClearOldKeys ) {

    // save temp copies first to allow caller to pass our own members in to us
    char *tempDirectoryName = stringDuplicate( inDirectoryName );
    
    if( mDirectoryName != NULL ) {
        delete [] mDirectoryName;
        }    

    mDirectoryName = tempDirectoryName;


    char *newLanguageName = stringDuplicate( inLanguageName );
    
    if( inClearOldKeys ) {
        char *tempLanguageName = stringDuplicate( inLanguageName );
        
        if( mLanguageName != NULL ) {
            delete [] mLanguageName;
            }
        
        mLanguageName = tempLanguageName;    
        }


    char dataSet = false;
    

    File *directoryFile = new File( NULL, mDirectoryName );

    if( directoryFile->exists() && directoryFile->isDirectory() ) {

        char *languageFileName = autoSprintf( "%s.txt", newLanguageName );
        
        File *languageFile = directoryFile->getChildFile( languageFileName );

        delete [] languageFileName;


        if( languageFile != NULL ) {


            char *languageData = languageFile->readFileContents();
            
            if( languageData != NULL ) {
                
                dataSet = true;

                setTranslationData( languageData, inClearOldKeys );
                delete [] languageData;
                }
            delete languageFile;
            }
        }

    delete directoryFile;
    
    delete [] newLanguageName;
    
    
    if( !dataSet ) {
        // failed to open file...
        
        // set blank data to ensure that vectors are created
        setTranslationData( "", inClearOldKeys );
        }
    
    }


static inline char *stringSkip( const char *inString, int inNumChars ) {
    return (char *)( &( inString[ inNumChars ] ) );
    }




void TranslationManagerStaticMembers::setTranslationData(
    const char *inData,
    char inClearOldKeys ) {
    
    if( inClearOldKeys ) {
        
        // clear the old translation table
        if( mTranslationKeys != NULL ) {
            int numKeys = mTranslationKeys->size();
            
            for( int i=0; i<numKeys; i++ ) {
                delete [] *( mTranslationKeys->getElement( i ) );
                }
            delete mTranslationKeys;
            mTranslationKeys = NULL;
            }
        
        if( mNaturalLanguageStrings != NULL ) {
            int numKeys = mNaturalLanguageStrings->size();
            
            for( int i=0; i<numKeys; i++ ) {
                delete [] *( mNaturalLanguageStrings->getElement( i ) );
                }
            delete mNaturalLanguageStrings;
            mNaturalLanguageStrings = NULL;
            }
        }
    
    if( mTranslationKeys == NULL ) {
        mTranslationKeys = new SimpleVector<char *>();
        mNaturalLanguageStrings = new SimpleVector<char *>();
        }
    
    
    // now read in the translation table

    
    char readError = false;
                
    while( ! readError ) {

        char *key = new char[ 100 ];

        int numRead = sscanf( inData, "%99s", key );

        if( numRead == 1 ) {
            
            inData = stringSkip( inData, strlen( key ) );
            
            
            // skip the first "
            int readChar = ' ';

            while( readChar != '"' && readChar != '\0' ) {
                readChar = inData[0];
                inData = stringSkip( inData, 1 );
                }
            if( readChar != EOF ) {
                char *naturalLanguageString = new char[1000];
                // scan a string of up to 999 characters, stopping
                // at the first " character
                numRead = sscanf( inData, "%999[^\"]",
                                  naturalLanguageString );

                if( numRead == 1 ) {
                    inData = stringSkip( inData, 
                                         strlen( naturalLanguageString ) );
                    
                    // only insert strings for keys that don't
                    // already exist
                    char keyExists = false;
                    
                    char *trimmedKey = stringDuplicate( key );

                    for( int i=0; i<mTranslationKeys->size(); i++ ) {
                        if( strcmp( *(mTranslationKeys->getElement(i) ),
                                    trimmedKey ) == 0 ) {
                            keyExists = true;
                            break;
                            }
                        }
                    
                    if( !keyExists ) {
                        
                        // trim the key and string and save them
                        mTranslationKeys->push_back( trimmedKey );
                        mNaturalLanguageStrings->push_back(
                            stringDuplicate( naturalLanguageString ) );
                        }
                    else {
                        delete [] trimmedKey;
                        }
                    }
                else {
                    readError = true;
                    }
                delete [] naturalLanguageString;
                
                // skip the trailing "
                readChar = ' ';
                
                while( readChar != '"' && readChar != '\0' ) {
                    readChar = inData[0];
                    inData = stringSkip( inData, 1 );                
                    }
                }
            else {
                readError = true;
                }
            
            }
        else {
            readError = true;
            }
        delete [] key;
        
        }
    }
