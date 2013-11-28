<?php



global $sg_version;
$sg_version = "1";



// edit settings.php to change server' settings
include( "settings.php" );




// no end-user settings below this point


// no caching
//header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache'); 



// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );



// page layout for web-based setup
$setup_header = "
<HTML>
<HEAD><TITLE>SteamGate Server Web-based setup</TITLE></HEAD>
<BODY BGCOLOR=#FFFFFF TEXT=#000000 LINK=#0000FF VLINK=#FF0000>

<CENTER>
<TABLE WIDTH=75% BORDER=0 CELLSPACING=0 CELLPADDING=1>
<TR><TD BGCOLOR=#000000>
<TABLE WIDTH=100% BORDER=0 CELLSPACING=0 CELLPADDING=10>
<TR><TD BGCOLOR=#EEEEEE>";

$setup_footer = "
</TD></TR></TABLE>
</TD></TR></TABLE>
</CENTER>
</BODY></HTML>";






// ensure that magic quotes are OFF
// we hand-filter all _REQUEST data with regexs before submitting it to the DB
if( get_magic_quotes_gpc() ) {
    // force magic quotes to be removed
    $_GET     = array_map( 'sg_stripslashes_deep', $_GET );
    $_POST    = array_map( 'sg_stripslashes_deep', $_POST );
    $_REQUEST = array_map( 'sg_stripslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'sg_stripslashes_deep', $_COOKIE );
    }
    





// all calls need to connect to DB, so do it once here
//sg_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = sg_requestFilter( "action", "/[A-Z_]+/i" );

$debug = sg_requestFilter( "debug", "/[01]/" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }




if( $action == "version" ) {
    global $sg_version;
    echo "$sg_version";
    }
else if( $action == "show_steam_login_page" ) {
    sg_showSteamLoginPage();
    }
else if( $action == "steam_login_return" ) {
    sg_steamLoginReturn();
    }






function sg_showSteamLoginPage() {
    global $steamLoginURL, $serverRootURL, $fullServerURL, $steamButtonURL;

    // Nonce needed here?  No, because Steam's response contains a nonce
    $returnURL = $fullServerURL . "?action=steam_login_return";


    $queryParams = array();

    $queryParams[ 'openid.mode' ] = "checkid_setup";
    $queryParams[ 'openid.ns' ] = "http://specs.openid.net/auth/2.0";
    $queryParams[ 'openid.return_to' ] = $returnURL;
    $queryParams[ 'openid.realm' ] = $serverRootURL;
    $queryParams[ 'openid.identity' ] =
        "http://specs.openid.net/auth/2.0/identifier_select";
    $queryParams[ 'openid.claimed_id' ] =
        "http://specs.openid.net/auth/2.0/identifier_select";
    

    $formattedParams = http_build_query( $queryParams );
    
    $buttonURL =
        $steamLoginURL . "?$formattedParams";
    
    echo "<a href='$buttonURL'><img border=0 src='$steamButtonURL'></a>";
    }



function sg_steamLoginReturn() {
    // php replaces all . with _ in URL parameter names!

    
    $signed = sg_requestFilter( "openid_signed", "/.*/" );
    $sig = sg_requestFilter( "openid_sig", "/.*/" );
    $ns = sg_requestFilter( "openid_ns", "/.*/" );
    $claimed_id = sg_requestFilter( "openid_claimed_id", "/.*/" );

    preg_match( "#http://steamcommunity.com/openid/id/(\d+)#",
                $claimed_id, $matches );
    
    $steamID = $matches[1];

    echo "SteamID = $steamID<br><br>";
    
    
    
    // all other params that have been signed
    $signedList = explode( ",", $signed );
    
    echo "signed = $signed<br>";
    echo "sig = $sig<br>";
    echo "ns = $ns<br>";
    
    $queryParams = array();

    foreach( $signedList as $paramName ) {
        $fullParamName = "openid_" . str_replace( ".", "_", $paramName );

        $paramValue = sg_requestFilter( "$fullParamName", "/.*/" );
        
        $queryParams["openid.$paramName"] = $paramValue;
        }

    $queryParams[ 'openid.mode' ] = "check_authentication";
    $queryParams[ 'openid.ns' ] = $ns;
    $queryParams[ 'openid.sig' ] = $sig;
    

    $formattedParams = http_build_query( $queryParams );
    
    
    // url to verify this with steam:

    global $steamLoginURL;

    echo "<br><br>Full formatted param string = $formattedParams<br>";
    
    
    $verifyURL = "$steamLoginURL?$formattedParams";

    $verifyResult = file_get_contents( $verifyURL );

    echo "<br><br>Result from Steam verify:  $verifyResult<br>";

    if( preg_match( "/is_valid\s*:\s*true/i", $verifyResult ) ) {
        echo "<br>Validated<br>";
        }
    else {
        echo "<br>Validation failed<br>";
        }
    
    
    }


/**
 * Recursively applies the stripslashes function to arrays of arrays.
 * This effectively disables magic_quote escaping behavior. 
 *
 * @inValue the value or array to stripslashes from.
 *
 * @return the value or array with slashes removed.
 */
function sg_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'ts_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function sg_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
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

?>