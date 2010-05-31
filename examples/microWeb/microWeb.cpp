/*
 * Modification History
 *
 * 2001-May-11   Jason Rohrer
 * Created.  
 */

#include "minorGems/io/file/File.h"
#include "WebServer.h"

#include <string.h>
#include <stdio.h>


#define DEFAULT_CONFIG_FILE "microWeb.config"

void usage( char *inAppName );


int main( char inNumArgs, char **inArgs ) {

	if( inNumArgs > 2 ) {
		usage( inArgs[0] );
		}

	char *fileNameBuffer = new char[99];
	
	if( inNumArgs == 1 ) {
		// no config file specified... use default
		printf( "using default:  config_file = %s\n", DEFAULT_CONFIG_FILE );

		// make a non-const version of the config file name
		sprintf( fileNameBuffer, "%s", DEFAULT_CONFIG_FILE );
		}
	else {
		// read config file name from command line

		sprintf( fileNameBuffer, "%s", inArgs[1] );
		}

	File *configFile = new File( NULL, fileNameBuffer,
								 strlen( fileNameBuffer ) );

	delete [] fileNameBuffer;
	
	WebServer *server = new WebServer( configFile );

	server->runServer();

	delete server;
	}



void usage( char *inAppName ) {

	printf( "Usage:\n" );
	printf( "\t%s [config_file]\n", inAppName );

	printf( "Examples:\n" );
	printf( "\t%s\n", inAppName );
	printf( "\t%s microWeb.config\n", inAppName );
	
	exit( 1 );
	}
