
#include "minorGems/io/file/File.h"

#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/log/FileLog.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/system/Time.h"


#include <windows.h>
#include <process.h>



static void launchGame( char *launchName ) {
    AppLog::infoF( "Launching app %s", launchName );
    char *arguments[2] = { (char*)launchName, NULL };
    
    AppLog::infoF( "Waiting for child process to exit after launching." );
    double startTime = Time::getCurrentTime();
    
    _spawnvp( _P_WAIT, launchName, arguments );
    
    AppLog::infoF( "Child game process ran for %f minutes before exiting.",
                   ( Time::getCurrentTime() - startTime ) / 60 );
    }



int main() {

    AppLog::setLog( new FileLog( "log_resSetter.txt" ) );
    AppLog::setLoggingLevel( Log::DETAIL_LEVEL );

    
    File launchFile( NULL, "resSetterLaunch.txt" );
    
    if( ! launchFile.exists() || launchFile.isDirectory() ) {
        AppLog::error( "resSetterLaunch.txt not found, quitting." );
        return 1;
        }

    int defaultWidth = 640;
    
    File defaultWidthFile( NULL, "resSetterDefaultWidth.txt" );
    
    if( defaultWidthFile.exists() ) {
        defaultWidth = defaultWidthFile.readFileIntContents( 640 );
        AppLog::infoF( "Read %d from resSetterDefaultWidth.txt", defaultWidth );
        }
    else {
        AppLog::errorF( "resSetterDefaultWidth.txt does not exist, "
                        "using %d as defaultWidth", defaultWidth );
        }

    
    char *launchName = launchFile.readFileContents();
    
    if( launchName == NULL ) {
        AppLog::error( 
            "Failed to read app name from resSetterLaunch.txt, quitting." );
        return 1;
        }

    
    File settingsFolder( NULL, "settings" );
    

    if( settingsFolder.exists() && settingsFolder.isDirectory() ) {
        
        char found = false;
        
        int manualRes = 
            SettingsManager::getIntSetting( "manualScreenResolution", &found );
        
        if( !found ) {
            SettingsManager::setSetting( "manualScreenResolution", 0 );
            manualRes = 0;
            }
        
        if( manualRes ) {
            AppLog::info( "settings/manualScreenResolution.ini set to 1, "
                          "launching without setting resoluton." );
            }
        else {
            // set resolution automatically based on native screen size
            
            int nativeX = GetSystemMetrics( SM_CXSCREEN );
            int nativeY = GetSystemMetrics( SM_CYSCREEN );
            
            AppLog::infoF( "Found native resolution %d x %d",
                           nativeX, nativeY );
            
            SettingsManager::setSetting( "screenWidth", nativeX );
            SettingsManager::setSetting( "screenHeight", nativeY );
            
            SettingsManager::setSetting( "fullscreen", 1 );

            
            double mouseSpeed = 
                SettingsManager::getDoubleSetting( "mouseSpeed", &found );

            if( !found ) {
                AppLog::infoF( "No mouseSpeed settings file, leaving it unset",
                               mouseSpeed );
                }
            else if( mouseSpeed == 1.0 ) {
                AppLog::info( "Default mouseSpeed of 1.0 found" );
                
                double newMouseSpeed = 
                    (double) defaultWidth / (double) nativeX;
                
                AppLog::infoF( "Setting new mouse speed based on "
                               "screen width to %f", newMouseSpeed );
                
                SettingsManager::setDoubleSetting( "mouseSpeed", 
                                                   newMouseSpeed );
                }
            else {
                AppLog::infoF( "Non-default mouse speed of %f found, "
                               "leaving it alone", mouseSpeed );
                }
            }
        }
    else {
        AppLog::error( "settings directory not found, "
                       "launching without setting resoluton or mouse speed." );
        }
    
    launchGame( launchName );
        
    delete [] launchName;
    
    return 1;
    }
