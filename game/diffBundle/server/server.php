<?php

global $dbs_version;
$dbs_version = "3";


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
        else if( preg_match( "/(\d+)_inc_$inPlatform.dbz/",
                             $file, $matches ) ) {
            if( $matches[1] > $latestVersion ) {
                $latestVersion = $matches[1];
                }
            }
        else if( preg_match( "/(\d+)_full_$inPlatform"."_urls.txt/",
                        $file, $matches ) ) {

            if( $matches[1] > $latestVersion ) {
                $latestVersion = $matches[1];
                }
            }
        else if( preg_match( "/(\d+)_inc_$inPlatform"."_urls.txt/",
                             $file, $matches ) ) {
            if( $matches[1] > $latestVersion ) {
                $latestVersion = $matches[1];
                }
            }
        }
    return $latestVersion;
    }


function dbs_getLatestVersionAll( $inPlatform ) {
    $latest = dbs_getLatestVersion( $inPlatform );
    $latestAll = dbs_getLatestVersion( "all" );

    if( $latestAll > $latest ) {
        $latest = $latestAll;
        }

    global $downloadFilePath;

    $latestLimitPath = $downloadFilePath . "latest.txt";

    if( file_exists( $latestLimitPath ) ) {
        
        $latestLimit = trim( file_get_contents( $latestLimitPath ) );

        if( $latest > $latestLimit ) {
            $latest = $latestLimit;
            }
        }
    
    return $latest;
    }




// returns 1 or 0 based on user-supplied vars
function dbs_isUpdateAvailable() {
    global $downloadFilePath;

    $platform = dbs_getPlatform();
    $oldVersion = dbs_getOldVersion();    

    $latest = dbs_getLatestVersionAll( $platform );

    if( $latest > $oldVersion ) {

        if( file_exists( $downloadFilePath .
                         "$latest"."_inc_$platform"."_urls.txt" )
            ||
            file_exists( $downloadFilePath .
                         "$latest"."_inc_all"."_urls.txt" ) ) {
            $result = "URLS\n#";
            
            for( $i=$oldVersion+1; $i <= $latest; $i++ ) {
                $a = $i - 1;
                $b = $i;

                $fileContents = FALSE;
                $filePath;
                
                if( file_exists( $downloadFilePath .
                                 "$i"."_inc_$platform"."_urls.txt" ) ) {

                    $filePath =
                        $downloadFilePath . "$i"."_inc_$platform"."_urls.txt";
                    
                    $fileContents = file_get_contents( $filePath );
                    }

                if( $fileContents === FALSE ) {
                    // failed to read
                    $filePath =
                        $downloadFilePath . "$i"."_inc_all_urls.txt";
                    
                    $fileContents = file_get_contents( $filePath );
                    }

                
                if( $fileContents === FALSE ) {
                    // failed to read either
                    }
                else {
                    $dbzPath = preg_replace('/_urls.txt/', '.dbz', $filePath );
                    
                    $size = filesize( $dbzPath );
                    $result = $result . "\nUPDATE $a->$b $size\n";
                    $result = $result . trim( $fileContents );
                    if( $i != $latest ) {
                        $result = $result . "\n#";
                        }
                    }
                }
            return $result;
            }
        else {
            $updateName;
            
            if( $latest == $oldVersion + 1 ) {
                $updateName = "$latest"."_inc_$platform".".dbz";

                if( filesize( $downloadFilePath . $updateName ) === FALSE ) {
                    $updateName = "$latest"."_inc_all".".dbz";
                    }
                }
            else {
                $updateName = "$latest"."_full_$platform".".dbz";

                if( filesize( $downloadFilePath . $updateName ) === FALSE ) {
                    $updateName = "$latest"."_full_all".".dbz";
                    }
                }
            
            return filesize( $downloadFilePath . $updateName );
            }
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

        if( filesize( $downloadFilePath . $updateName ) === FALSE ) {
            $updateName = "$latest"."_inc_all".".dbz";
            }
        }
    else {
        $updateName = "$latest"."_full_$platform".".dbz";

        if( filesize( $downloadFilePath . $updateName ) === FALSE ) {
            $updateName = "$latest"."_full_all".".dbz";
            }
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