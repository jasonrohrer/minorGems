<?php

global $dbs_version;
$dbs_version = "2";


// edit settings.php to change server' settings
include( "settings.php" );


// no caching
//header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );



$action = dbs_requestFilter( "action", "/[A-Z_]+/i" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }



if( $action == "version" ) {
    global $dbs_version;
    echo "$dbs_version";
    }
else if( $action == "is_update_available" ) {
    $result = dbs_isUpdateAvailable();
    echo "$result";
    }
else if( $action == "get_update" ) {
    dbs_getUpdate();
    }



function dbs_getPlatform() {
    return dbs_requestFilter( "platform", "/[a-z0-9]+/i" );
    }



function dbs_getOldVersion() {
    return dbs_requestFilter( "old_version", "/[0-9]+/i" );
    }



function dbs_getLatestVersion( $inPlatform ) {
    global $downloadFilePath;

    $fileList = scandir( $downloadFilePath );

    $latestVersion = 0;
    foreach( $fileList as $file ) {
        if( preg_match( "/(\d+)_full_$inPlatform.dbz/",
                        $file, $matches ) ) {

            if( $matches[1] > $latestVersion ) {
                $latestVersion = $matches[1];
                }
            }
        }
    return $latestVersion;
    }



// returns 1 or 0 based on user-supplied vars
function dbs_isUpdateAvailable() {
    $platform = dbs_getPlatform();
    $oldVersion = dbs_getOldVersion();    

    $latest = dbs_getLatestVersion( $platform );
    
    if( $latest > $oldVersion ) {

        $updateName;
        
        if( $latest == $oldVersion + 1 ) {
            $updateName = "$latest"."_inc_$platform".".dbz";
            }
        else {
            $updateName = "$latest"."_full_$platform".".dbz";
            }

        global $downloadFilePath;

        return filesize( $downloadFilePath . $updateName );
        }
    else {
        return 0;
        }
    }


function dbs_getUpdate() {
    $platform = dbs_getPlatform();
    $oldVersion = dbs_getOldVersion();
    
    $latest = dbs_getLatestVersion( $platform );

    if( $latest <= $oldVersion ) {
        echo "DENIED";
        return;
        }

    $updateName;
    
    if( $latest == $oldVersion + 1 ) {
        $updateName = "$latest"."_inc_$platform".".dbz";
        }
    else {
        $updateName = "$latest"."_full_$platform".".dbz";
        }
    
    global $downloadFilePath;

    
    $result = dbs_send_file( $downloadFilePath . $updateName );


    if( ! $result ) {
        echo "DENIED";
        return;
        }
    }

    


/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function dbs_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
    if( ! isset( $_REQUEST[ $inRequestVariable ] ) ) {
        return $inDefault;
        }
    
    $numMatches = preg_match( $inRegex,
                              $_REQUEST[ $inRequestVariable ], $matches );

    if( $numMatches != 1 ) {
        return $inDefault;
        }
        
    return $matches[0];
    }




// found here:
// http://php.net/manual/en/function.fpassthru.php

function dbs_send_file( $path ) {
    session_write_close();
    //ob_end_clean();
    
    if( !is_file( $path ) || connection_status() != 0 ) {
        return( FALSE );
        }
    

    //to prevent long file from getting cut off from     //max_execution_time

    set_time_limit( 0 );

    $name = basename( $path );

    //filenames in IE containing dots will screw up the
    //filename unless we add this

    // sometimes user agent is not set!
    if( ! empty( $_SERVER['HTTP_USER_AGENT'] ) ) {
        
        if( strstr( $_SERVER['HTTP_USER_AGENT'], "MSIE" ) ) {
            $name =
                preg_replace('/\./', '%2e',
                             $name, substr_count($name, '.') - 1);
            }
        }
    
    
    //required, or it might try to send the serving
    //document instead of the file

    header("Cache-Control: ");
    header("Pragma: ");
    header("Content-Type: application/octet-stream");
    header("Content-Length: " .(string)(filesize($path)) );
    header('Content-Disposition: attachment; filename="'.$name.'"');
    header("Content-Transfer-Encoding: binary\n");

    if( $file = fopen( $path, 'rb' ) ) {
        while( ( !feof( $file ) )
               && ( connection_status() == 0 ) ) {
            print( fread( $file, 1024*8 ) );
            flush();
            }
        fclose($file);
        }
    return( (connection_status() == 0 ) and !connection_aborted() );
    }

?>