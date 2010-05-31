/*
 * Modification History
 *
 * 2010-March-23   Jason Rohrer
 * Created.
 *
 * 2010-March-24   Jason Rohrer
 * Fixed a destruction bug when discovery fails.
 *
 * 2010-April-6   Jason Rohrer
 * Switched from printf to log.
 *
 * 2010-May-14    Jason Rohrer
 * String parameters as const to fix warnings.
 */


#include "portMapping.h"


#include <stdio.h>
#include <string.h>

#include "miniupnpc/miniupnpc.h"
#include "miniupnpc/upnpcommands.h"
#include "miniupnpc/upnperrors.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"


static int getIGDURL( int inTimeoutMS, 
                      struct UPNPUrls *urls,
                      struct IGDdatas *data,
                      char *lanIPAddr,
                      int lanIPLength ) {

    AppLog::info( "Trying to do UPNP discover\n" );
    struct UPNPDev *devList = upnpDiscover( inTimeoutMS, NULL, NULL, 0 );

    if( devList == NULL ) {
        AppLog::error( "UPNP discovery failed\n" );
        return -1;
        }    

    AppLog::info( "Checking UPNP dev list for a Gateway\n" );
    
    int result = 
        UPNP_GetValidIGD( devList, urls, data, 
                          lanIPAddr, lanIPLength );

    freeUPNPDevlist( devList );
        

    if( result != 0 ) {
        if( result != 1 ) {
            AppLog::error( "No connected UPNP Gateway Device found\n" );
            }
        }
    else {
        AppLog::error( "No UPNP Gateway Device found\n" );
        }

    return result;
    }



int mapPort( int inPort, const char *inDescription,
             int inTimeoutMS,
             char **outExternalIP ) {

    *outExternalIP = NULL;
    int returnVal = -1;


    struct UPNPUrls urls;
    struct IGDdatas data;

    char lanIPAddr[16];

    int result = getIGDURL( inTimeoutMS, 
                            &urls, &data, 
                            lanIPAddr, 16 );

    if( result > 0 ) {
        
        if( result == 1 ) {
            
            
            char externalIPAddress[16];
            

            int result = UPNP_GetExternalIPAddress( urls.controlURL,
                                       data.servicetype,
                                       externalIPAddress );
            
            if( result == UPNPCOMMAND_SUCCESS &&
                externalIPAddress[0] != '\0' ) {
                
                *outExternalIP = stringDuplicate( externalIPAddress );
                }
            else {
                AppLog::error( "GetExternalIPAddress failed.\n" );
                *outExternalIP = NULL;
                }
            
            char *port = autoSprintf( "%d", inPort );            
	
            result = 
                UPNP_AddPortMapping( urls.controlURL, data.servicetype,
                                     port, port, lanIPAddr,
                                     inDescription, "TCP", NULL );
            if( result != UPNPCOMMAND_SUCCESS ) {
                
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "AddPortMapping(%s, %s, %s) failed with code "
                    "%d (%s)\n",
                    port, port, lanIPAddr, result, strupnperror(result) );
                }
            else {
                AppLog::info( "AddPortMapping success\n" ); 
                returnVal = 1;
                }

            delete [] port;
            }
    
        FreeUPNPUrls( &urls );
        }
    

    return returnVal;
    }



int unmapPort( int inPort, int inTimeoutMS ) {
    int returnVal = -1;

    
    struct UPNPUrls urls;
    struct IGDdatas data;

    char lanIPAddr[16];
    
    int result = getIGDURL( inTimeoutMS, 
                            &urls, &data, 
                            lanIPAddr, 16 );
    
    if( result > 0 ) {
        
        if( result == 1 ) {
            
            
            char *port = autoSprintf( "%d", inPort );            
	
            int result =
                UPNP_DeletePortMapping( urls.controlURL, data.servicetype,
                                        port, "TCP", NULL );
            
            if( result != UPNPCOMMAND_SUCCESS ) {
                
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "DeletePortMapping(%s) failed with code "
                    "%d (%s)\n",
                    port, result, strupnperror(result) );
                }
            else {
                AppLog::info( "DeletePortMapping success\n" );
                returnVal = 1;
                }

            delete [] port;
            }
    
        FreeUPNPUrls( &urls );
        }
    

    return returnVal;
    }

