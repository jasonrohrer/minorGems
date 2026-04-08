<?php



global $ds_version;
$ds_version = "1";



// edit settings.php to change server' settings
include_once( "settings.php" );
include_once( "discord.php" );




// no end-user settings below this point









// no caching
//header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache'); 



// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );


// for stack trace of errors
/*
set_error_handler(function($severity, $message, $file, $line) {
    if (error_reporting() & $severity) {
        throw new ErrorException($message, 0, $severity, $file, $line);
    }
});
*/



global $ds_emailRegex;
$ds_emailRegex = "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i";



// page layout for web-based setup
$setup_header = "
<HTML>
<HEAD><TITLE>Discord Steam Server Web-based setup</TITLE></HEAD>
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
if( function_exists( "get_magic_quotes_gpc" ) && get_magic_quotes_gpc() ) {
    // force magic quotes to be removed
    $_GET     = array_map( 'ds_stripslashes_deep', $_GET );
    $_POST    = array_map( 'ds_stripslashes_deep', $_POST );
    $_REQUEST = array_map( 'ds_stripslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'ds_stripslashes_deep', $_COOKIE );
    }
    


// Check that the referrer header is this page, or kill the connection.
// Used to block XSRF attacks on state-changing functions.
// (To prevent it from being dangerous to surf other sites while you are
// logged in as admin.)
// Thanks Chris Cowan.
function ds_checkReferrer() {
    global $fullServerURL;
    
    if( !isset($_SERVER['HTTP_REFERER']) ||
        strpos($_SERVER['HTTP_REFERER'], $fullServerURL) !== 0 ) {
        
        die( "Bad referrer header" );
        }
    }




// all calls need to connect to DB, so do it once here
ds_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = ds_requestFilter( "action", "/[A-Z0-9_]+/i" );

$debug = ds_requestFilter( "debug", "/[01]/" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }


$showHeaderAndFooter = false;

// pages that are shown to end user have header and footer
if( $action == "steam_login_return" ||
    $action == "unlock_discord" ) {

    $showHeaderAndFooter = true;
    
    global $header;
    eval( $header );
    }


$logAllRequests = false;

if( $logAllRequests ) {
    $postBody = file_get_contents( 'php://input' );

    $req = var_export( $_REQUEST, true );
    ds_log( "Request ($remoteIP):\n$req\n$postBody" );
    }



if( $action == "version" ) {
    global $ds_version;
    echo "$ds_version";
    }
else if( $action == "steam_login_return" ) {
    ds_steamLoginReturn();
    }
else if( $action == "login" ) {
    ds_clientLogin();
    }
else if( $action == "show_log" ) {
    ds_showLog();
    }
else if( $action == "clear_log" ) {
    ds_clearLog();
    }
else if( $action == "show_data" ) {
    ds_showData();
    }
else if( $action == "show_detail" ) {
    ds_showDetail();
    }
else if( $action == "logout" ) {
    ds_logout();
    }
else if( $action == "unlock_discord" ) {
    ds_unlockDiscord();
    }
else if( $action == "discord_interaction" ) {
    ds_discordInteraction();
    }
else if( $action == "ds_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Discord Steam Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    ds_setupDatabase();

    echo "</TD></TR></TABLE></TD></TR></TABLE></CENTER><BR><BR>";
    
    echo $setup_footer;
    }
else if( preg_match( "/server\.php/", $_SERVER[ "SCRIPT_NAME" ] ) ) {
    // server.php has been called without an action parameter

    // the preg_match ensures that server.php was called directly and
    // not just included by another script
    
    // quick (and incomplete) test to see if we should show instructions
    global $tableNamePrefix;
    
    // check if our tables exist
    $exists =
        ds_doesTableExist( $tableNamePrefix . "random_nouns" ) &&
        ds_doesTableExist( $tableNamePrefix . "users" ) &&
        ds_doesTableExist( $tableNamePrefix . "log" );
    
        
    if( $exists  ) {
        echo "Discord Steam Server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Skydrop Server Web-based Setup</H2>";
    
        echo "Skydrop Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=ds_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// pages that are shown to end user have header and footer
if( $showHeaderAndFooter ) {

    global $footer;
    eval( $footer );
    }



// done processing
// only function declarations below

ds_closeDatabase();








/**
 * Creates the database tables needed by seedBlogs.
 */
function ds_setupDatabase() {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . "log";
    if( ! ds_doesTableExist( $tableName ) ) {

        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL, ".
            "index( entry_time ) );";

        $result = ds_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    

    // these words taken from a cognitive experiment database
    // http://www.datavis.ca/online/paivio/
    // Now moved here:
    // http://euclid.psych.yorku.ca/shiny/Paivio/

    $tableName = $tableNamePrefix . "random_nouns";
    if( ! ds_doesTableExist( $tableName ) ) {

        // a source list of character last names
        // cumulative count is number of people in 1993 population
        // who have this name or a more common name
        // less common names have higher cumulative counts
        $query =
            "CREATE TABLE $tableName( " .
            "id SMALLINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, ".
            "noun VARCHAR(14) NOT NULL, ".
            "UNIQUE KEY( noun ) );";

        $result = ds_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";


        if( $file = fopen( "randomNouns.txt", "r" ) ) {
            $firstLine = true;

            $query = "INSERT INTO $tableName (noun) VALUES ";
            /*
			( 'bird' ),
            ( 'monster' ),
            ( 'ability' );
            */

            while( !feof( $file ) ) {
                $noun = trim( fgets( $file) );
                
                if( $noun != "" ) {
                    if( ! $firstLine ) {
                        $query = $query . ",";
                        }
                    
                    $query = $query . " ( '$noun' )";
                    
                    $firstLine = false;
                    }
                }
            
            fclose( $file );

            $query = $query . ";";
            
            $result = ds_queryDatabase( $query );
            }
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "users";
    if( ! ds_doesTableExist( $tableName ) ) {

        $query =
            "CREATE TABLE $tableName(" .
            "steam_id VARCHAR(255) NOT NULL PRIMARY KEY," .
            "creation_date DATETIME NOT NULL, ".
            // separated by spaces
            "discord_unlock_words VARCHAR(254) NOT NULL," .
            "UNIQUE KEY( discord_unlock_words ), ".
            "discord_unlock_words_used tinyint NOT NULL, ".
            "discord_user_id VARCHAR(254) NOT NULL, ".
            "discord_dm_channel_id VARCHAR(254) NOT NULL );";

        $result = ds_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }




function ds_showLog() {
    ds_checkPassword( "show_log" );

    ds_showLinkHeader();

    $entriesPerPage = 1000;
    
    $skip = ds_requestFilter( "skip", "/\d+/", 0 );

    
    global $tableNamePrefix;


    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."log;";

    $result = ds_queryDatabase( $query );
    $totalEntries = ds_mysqli_result( $result, 0, 0 );


    
    $query = "SELECT entry, entry_time FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC LIMIT $skip, $entriesPerPage;";
    $result = ds_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );



    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $entriesPerPage - 1;

    if( $endSkip > $totalEntries ) {
        $endSkip = $totalEntries;
        }

    

    
    echo "$totalEntries Log entries".
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $entriesPerPage;

    $prevSkip = $skip - $entriesPerPage;

    if( $skip > 0 && $prevSkip < 0 ) {
        $prevSkip = 0;
        }
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_log" .
            "&skip=$prevSkip\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalEntries ) {
        echo "[<a href=\"server.php?action=show_log" .
            "&skip=$nextSkip\">".
            "Next Page</a>]";
        }
    
        
    echo "<hr>";

        
    
    for( $i=0; $i<$numRows; $i++ ) {
        $time = ds_mysqli_result( $result, $i, "entry_time" );
        $entry = htmlspecialchars( ds_mysqli_result( $result, $i, "entry" ) );

        echo "<b>$time</b>:<br><pre>$entry</pre><hr>\n";
        }

    echo "<br><br><hr><a href=\"server.php?action=clear_log\">".
        "Clear log</a>";
    }



function ds_clearLog() {
    ds_checkPassword( "clear_log" );

    ds_showLinkHeader();

    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = ds_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }






function ds_logout() {
    ds_checkReferrer();
    
    ds_clearPasswordCookie();

    echo "Logged out";
    }



function ds_showLinkHeader() {
    
    echo "<table width='100%' border=0><tr>".
        "<td>[<a href=\"server.php?action=show_data" .
        "\">Main</a>]</td>".
        "<td align=right>[<a href=\"server.php?action=export_emails" .
        "\">Export Emails</a>][<a href=\"server.php?action=logout" .
        "\">Logout</a>]</td>".
        "</tr></table><br><br><br>";
    }



function ds_showData( $checkPassword = true ) {
    // these are global so they work in embeded function call below
    global $skip, $search, $order_by;

    if( $checkPassword ) {
        ds_checkPassword( "show_data" );
        }
    
    global $tableNamePrefix, $remoteIP;
    

    ds_showLinkHeader();



    $skip = ds_requestFilter( "skip", "/[0-9]+/", 0 );
    
    global $usersPerPage;
    
    $search = ds_requestFilter( "search", "/[A-Z0-9_@. \-]+/i" );

    $order_by = ds_requestFilter( "order_by", "/[A-Z_]+/i",
                                  "steam_id" );
    $order_dir = ds_requestFilter( "order_dir", "/(DESC|ASC)/",
                                   "DESC" );
    
    $keywordClause = "";
    $searchDisplay = "";
    
    if( $search != "" ) {
        

        $keywordClause = "WHERE ( steam_id LIKE '%$search%' " .
            "OR discord_user_id LIKE '%$search%' ".
            "OR discord_unlock_words LIKE '%$search%' ) ";

        $searchDisplay = " matching <b>$search</b>";
        }
    

    

    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix".
        "users $keywordClause;";

    $result = ds_queryDatabase( $query );
    $totalRecords = ds_mysqli_result( $result, 0, 0 );    
             
    $query = "SELECT * ".
        "FROM $tableNamePrefix"."users $keywordClause".
        "ORDER BY $order_by $order_dir ".
        "LIMIT $skip, $usersPerPage;";
    $result = ds_queryDatabase( $query );
    
    $numRows = mysqli_num_rows( $result );

    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $usersPerPage - 1;

    if( $endSkip > $totalRecords ) {
        $endSkip = $totalRecords;
        }



        // form for searching users and resetting tokens
?>
        <hr>
             <table border=0 width=100%><tr><td>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="show_data">
    <INPUT TYPE="hidden" NAME="order_by" VALUE="<?php echo $order_by;?>">
    <INPUT TYPE="hidden" NAME="order_dir" VALUE="<?php echo $order_dir;?>">
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="search"
             VALUE="<?php echo $search;?>">
    <INPUT TYPE="Submit" VALUE="Search">
    </FORM>
             </td>
             <td align=right>
             </td>
             </tr>
             </table>
        <hr>
<?php

    

    
    echo "$totalRecords user records". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $usersPerPage;

    $prevSkip = $skip - $usersPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$prevSkip&search=$search".
            "&order_by=$order_by&order_dir=$order_dir\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalRecords ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$nextSkip&search=$search".
            "&order_by=$order_by&order_dir=$order_dir\">".
            "Next Page</a>]";
        }

    echo "<br><br>";
    
    echo "<table border=1 cellpadding=5>\n";

    function orderLink( $inOrderBy, $inLinkText, $inOrderDir ) {
        global $skip, $search, $order_by;
        if( $inOrderBy == $order_by ) {
            // already displaying this order, don't show bold link
            // that switches order_dir

            $swapDir = "DESC";
            if( $inOrderDir == "DESC" ) {
                $swapDir = "ASC";
                }
            
            return "<a href=\"server.php?action=show_data" .
            "&search=$search&skip=$skip&order_by=$inOrderBy".
                "&order_dir=$swapDir\"><b>$inLinkText</b></a>";
            }

        // else show a link to switch to this order
        return "<a href=\"server.php?action=show_data" .
            "&search=$search&skip=$skip&order_by=$inOrderBy\">$inLinkText</a>";
        }

    
    echo "<tr>\n";    
    echo "<tr><td>".orderLink( "steam_id", "Steam ID", $order_dir )."</td>\n";
    echo "<td>".orderLink( "discord_unlock_words", "Discord Words",
                           $order_dir )."</td>\n";
    echo "<td>".orderLink( "discord_unlock_words_used",
                           "Discord Unlocked?", $order_dir )."</td>\n";
   echo "<td>".orderLink( "creation_date", "Created", $order_dir )."</td>\n";
    echo "</tr>\n";


    for( $i=0; $i<$numRows; $i++ ) {
        $steam_id = ds_mysqli_result( $result, $i, "steam_id" );
    
        $discord_unlock_words =
            ds_mysqli_result( $result, $i, "discord_unlock_words" );
        $discord_unlock_words_used =
            ds_mysqli_result( $result, $i, "discord_unlock_words_used" );

        $creation_date =
            ds_mysqli_result( $result, $i, "creation_date" );
  
        echo "<tr>\n";

        echo "<td>$steam_id</td>\n";
        echo "<td>$discord_unlock_words</td>\n";
        echo "<td>$discord_unlock_words_used</td>\n";
        echo "<td>$creation_date</td>\n";
        echo "</tr>\n";
        }
    echo "</table>";



    echo "<hr>";
         
    echo "<a href=\"server.php?action=show_log\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    }







    







function ds_mysqlEscape( $inString ) {
    global $ds_mysqlLink;
    return mysqli_real_escape_string( $ds_mysqlLink, $inString );
    }






function ds_getDiscordUnlockWordsForSteamID( $inSteamID) {
    global $tableNamePrefix;
    
    $query = "SELECT discord_unlock_words FROM $tableNamePrefix"."users ".
        "WHERE steam_id = '$inSteamID';";
    $result = ds_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    if( $numRows < 1 ) {
        return "";
        }
    else {
        return ds_mysqli_result( $result, 0, "discord_unlock_words" );
        }
    }






// convert a binary string into an ascii "1001011"-style string
function ds_getBinaryDigitsString( $inBinaryString ) {
    $binaryDigits = str_split( $inBinaryString );

    // string of 0s and 1s
    $binString = "";
    
    foreach( $binaryDigits as $digit ) {
        $binDigitString = decbin( ord( $digit ) );

        // pad with 0s
        $binDigitString =
            substr( "00000000", 0, 8 - strlen( $binDigitString ) ) .
            $binDigitString;

        $binString = $binString . $binDigitString;
        }

    // now have full string of 0s and 1s for $inBinaryString
    return $binString;
    } 



function ds_getSecureRandomBoundedInt( $inMaxVal, $inSecret ) {
    $bitsNeeded = ceil( log( $inMaxVal, 2 ) );

    $intVal = $inMaxVal + 1;

    while( $intVal > $inMaxVal ) {

        // get enough digits to generate an int from it
        $binString = "";
        while( strlen( $binString ) < $bitsNeeded ) {
            $randVal = rand();
            
            $hash_bin =
                ds_hmac_sha1_raw( $inSecret,
                                  uniqid( "$randVal", true ) );

            $binString = $binString . ds_getBinaryDigitsString( $hash_bin );
            }
        $binString = substr( $binString, 0, $bitsNeeded );
        $intVal = bindec( $binString );
        }
    
    return $intVal;
    }




// keeps trying up to 100 times until it picks
// one that's not already assigned to another user
function ds_generateRandomPasswordSequence( $steam_id, $numberOfWords = 4 ) {

    global $tableNamePrefix;

    $name = "";

    $words = array();

    global $passWordSelectionSecret;

    $hit = true;
    $phrase = "";

    $tryCount = 0;
    
    while( $hit && $tryCount < 100 ) {    
        // collision with existing, try again
        $hit = false;
        
        $query = "SELECT COUNT(*) from $tableNamePrefix"."random_nouns;";
        $result = ds_queryDatabase( $query );
        $totalNouns = ds_mysqli_result( $result, 0, 0 );        
        
        while( count( $words ) < $numberOfWords ) {
            
            $pick = ds_getSecureRandomBoundedInt(
                $totalNouns - 1,
                $steam_id . $passWordSelectionSecret );
            
            $query = "SELECT noun from $tableNamePrefix"."random_nouns ".
                "limit $pick, 1;";
            $result = ds_queryDatabase( $query );
            $noun = ds_mysqli_result( $result, 0, 0 );
            
            $words[] = $noun;
            }
        $phrase = join( " ", $words ); 

        $query = "SELECT COUNT(*) from $tableNamePrefix"."users ".
            "WHERE discord_unlock_words = '$phrase';";
        $result = ds_queryDatabase( $query );
        $count = ds_mysqli_result( $result, 0, 0 );

        if( $count > 0 ) {
            $hit = true;
            $phrase = "FAILED";
            }
        
        $tryCount++;
        }    
        
        
    return $phrase;
    }






function getHTTPHeaders() {
    $headers = [];
    foreach( $_SERVER as $name => $value ) {
        if (substr($name, 0, 5) == 'HTTP_') {
            $headers[str_replace(' ', '-', ucwords(strtolower(
                 str_replace('_', ' ', substr($name, 5)))))] = $value;
            }
        }
    return $headers;
    }



// returns four secret words
function ds_createAccount( $steam_id  ) {
    global $tableNamePrefix;

    $discord_unlock_words = ds_generateRandomPasswordSequence( $steam_id );

    if( $discord_unlock_words != "FAILED" ) {
            
        $query =
            "INSERT INTO $tableNamePrefix"."users ".
            "SET steam_id = '$steam_id', ".
            "discord_unlock_words = '$discord_unlock_words', ".
            "discord_unlock_words_used = 0,".
            "discord_user_id = '0',".
            "discord_dm_channel_id = '0',".
            "creation_date = CURRENT_TIMESTAMP;";
            
        $result = ds_queryDatabase( $query );

        ds_log( "Creating user account for $steam_id" );
            
        return $discord_unlock_words;
        }
    else {
        ds_log( "FAILED: Creating user account for $steam_id.  ".
                "Error: failed to generate unique pass words sequence." );

        return "Failed to create account.";
        }
    }








function ds_getDiscordRoleName( $inRoleID ) {
    global $discordGuildID;
    
    $endpoint =
        "https://discordapp.com/api/guilds/$discordGuildID/roles";
                            
    $result = ds_discordAPICall( $endpoint, "GET" );
                            
    $roleName = "Unknown";
                            
    if( $result[0] == 200 ) {
        $a = json_decode( $result[1], true );
        foreach( $a as $role ) {
            if( $role[ "id" ] == $inRoleID ) {
                $roleName = $role[ "name" ];
                }
            }
        }

    return $roleName;
    }





// returns 0 if failed to make channel
// returns 1 if made channel but failed to send first message
// returns 2 if made channel and sent first message
function ds_openDMChannel( $steam_id, $discordID ) {
    
    // now try to open a DM channel with them
    $endpoint = "https://discordapp.com/api".
        "/users/@me/channels";
    
    $postBody = "{ \"recipient_id\": \"$discordID\" }";
    
    $result = ds_discordAPICall( $endpoint, "POST",
                                 $postBody );

    if( $result[0] == 200 ) {
        $a = json_decode( $result[1], true );

        if( $a != null ) {
            if( array_key_exists( 'id', $a ) ) {
                $channelID = $a[ "id" ];
                $madeChannel = true;

                global $tableNamePrefix;
                
                $query =
                    "UPDATE $tableNamePrefix"."users ".
                    "SET discord_dm_channel_id = ".
                    "'$channelID' ".
                    "WHERE steam_id = $steam_id";
                            
                ds_queryDatabase( $query );

                // now send them a message

                $messageSent =
                    ds_discordSendMessageToChannel(
                        $channelID,
                        "Private channel started." );

                if( $messageSent ) {
                    return 2;
                    }
                else {
                    return 1;
                    }
                }
            }
        }
    
    return 0;
    }



function ds_areSecretWordsValid( $secret_words ) {
    global $tableNamePrefix;

    $query = "SELECT steam_id ".
        "FROM $tableNamePrefix"."users ".
        "WHERE discord_unlock_words = '$secret_words';";
    
    $result = ds_queryDatabase( $query );
    $numRows = mysqli_num_rows( $result );
        
    if( $numRows == 1 ) {
        return true;
        }
    else {
        return false;
        }
    }



function ds_unlockDiscord() {

    echo "<br><br><center>Login with Steam to unlock the ".
        "Discord Members Area:<br><br>";
    
    ds_showSteamLoginButton();

    echo "</center><br><br>";
    }


function ds_discordInteraction() {
    global $discordPublicKey;
    
    $postBody = file_get_contents( 'php://input' );

    
    $result =
        ds_discordEndpointVerify( $_SERVER, $postBody, $discordPublicKey );

    if( ! $result ) {
        ds_log( "Discord endpoint verification failed, returning 401" );
        http_response_code( 401 );
        die;
        }

    $json = json_decode( $postBody );



    header( 'Content-type: application/json' );

    if( $json->{ 'type' } == 1 ) {
        // ping
        // send pong
        echo '{ "type":1 }';
        }
    else if( $json->{ 'type' } == 2 ) {
        // application command
        $commandName = $json->{ 'data' }->{ 'name' };

        $message = "";
        
        if( $commandName == "unlock" ) {

            $discordID;

            if( isset( $json->{ 'member' }->{ 'user' }->{ 'id' } ) ) {
                // back when people messaged the bot from a channel
                // their user ID would be in a member field
                $discordID =
                    $json->{ 'member' }->{ 'user' }->{ 'id' };
                }
            else if( isset( $json->{ 'user' }->{ 'id' } ) ) {
                // now that people are DM-ing the bot, there is no member
                // field
                $discordID =
                    $json->{ 'user' }->{ 'id' };
                }
            else {
                $message = "Discord User ID missing.";
                $response =
                    '{ "type":4, "data":{ "flags":64, "content":"' . $message .'" } }';

                echo $response;
                return;
                }
                        
            
            $optionName = $json->{ 'data' }->{ 'options' }[0]->{ 'name' };

            if( $optionName == "secret-words" ) {

                $optionValue = $json->{ 'data' }->{ 'options' }[0]->{ 'value' };
                
                // convert to lowercase with one space between
                $submittedWords = strtolower(
                    join( " ", preg_split( "/\s+/",
                                           trim( $optionValue ) ) ) );

                // run through valid check here
                // just to make sure it gets caught by same brute-force
                // slow-down code for bad guesses
                $valid = ds_areSecretWordsValid( $submittedWords );
                

                global $tableNamePrefix;
                
                $query = "SELECT steam_id, discord_unlock_words_used ".
                    "FROM $tableNamePrefix"."users ".
                    "WHERE discord_unlock_words = '$submittedWords';";

                $result = ds_queryDatabase( $query );
                $numRows = mysqli_num_rows( $result );
                
                if( $valid && $numRows == 1 ) {
                    
                    $steam_id = ds_mysqli_result( $result, 0, "steam_id" );
                    $discord_unlock_words_used =
                        ds_mysqli_result( $result, 0,
                                          "discord_unlock_words_used" );

                    if( ! $discord_unlock_words_used ) {
                        
                    
                        // upgrade their role
                        global $discordGuildID, $discordSpecialRoleID;
                    
                        $endpoint = "https://discordapp.com/api".
                            "/guilds/$discordGuildID".
                            "/members/$discordID/roles/$discordSpecialRoleID";

                        ds_log( "Unlocking discord access with this endpoint:".
                                "  $endpoint" );
                        
                        $result = ds_discordAPICall( $endpoint, "PUT" );
                        
                        $responseCode = $result[0];

                        
                        if( $responseCode == 204 ) {
                            
                            $query = "UPDATE $tableNamePrefix"."users ".
                                "SET discord_user_id = '$discordID', ".
                                "discord_unlock_words_used = 1 ".
                                "WHERE steam_id = $steam_id";
                            
                            ds_queryDatabase( $query );
                            
                            // now fetch name of role they were assigned
                            $roleName =
                                ds_getDiscordRoleName( $discordSpecialRoleID );
                            
                        
                            $message =
                                "Command **/$commandName** was successful.  ".
                                "You now have the **$roleName** role.";


                            $dmResult =
                                ds_openDMChannel( $steam_id, $discordID );

                            if( $dmResult == 2 ) {
                                
                                $message =  $message .
                                    "  A private message channel with ".
                                    "this bot should now exist.";
                                }
                            else if( $dmResult == 1 ) {
                                $message =  $message .
                                    "  Failed to send a private ".
                                    "message to you from this ".
                                    "bot.";
                                }
                            else {
                                $message =
                                    $message .
                                    "  NOTE:  A private message channel with ".
                                    "this bot could NOT be created.";
                                }
                            }
                        else {
                            $message = "Command **/$commandName** Error: ".
                                "Unexpected status code ($responseCode) ".
                                "from Discord.";
                            }
                        }
                    else {
                        $message = "Command **/$commandName** Error: ".
                                "Secret words have already been used.";
                        }
                    }
                else {
                    $message =
                        "Command **/$commandName** Error: ".
                        "secret words do not match.";
                    }
                }
            else {
                $message = "Command **/$commandName** Error: Secret words missing.";
                }
            }
        else {
            $message = "Command **/$commandName** unknown.";
            }
        
        $response = '{ "type":4, "data":{ "flags":64, "content":"' . $message .'" } }';

        ds_log( "Got discord interaction (type=2) with postBody:  $postBody".
                "\n\nSending response: $response" );
        
        echo $response;
        }
    else {
        // unexpected type
        $message = "Unexpected interaction received.";
        $response = '{ "type":4, "data":{ "flags":64, "content":"' . $message .'" } }';

        echo $response;
        }
    }




// returns true if verify succeeds
function ds_discordEndpointVerify( array $headers, $payload, $publicKey ) {

    if( ! isset( $headers['HTTP_X_SIGNATURE_ED25519'] )
        || ! isset( $headers['HTTP_X_SIGNATURE_TIMESTAMP'] ) ) {
        ds_log( "Discord request missing HTTP_X headers" );
        return false;
        }
    $signature = $headers[ 'HTTP_X_SIGNATURE_ED25519' ];
    $timestamp = $headers[ 'HTTP_X_SIGNATURE_TIMESTAMP' ];
    
    if( !trim( $signature, '0..9A..Fa..f' ) == '' ) {
        ds_log( "Trimming signature on Discord request failed" );
        return false;
        }
    
    $message = $timestamp . $payload;
    $binarySignature = sodium_hex2bin( $signature );
    $binaryKey = sodium_hex2bin( $publicKey );
    
    if( !sodium_crypto_sign_verify_detached( $binarySignature,
                                             $message, $binaryKey ) ) {
        ds_log( "Sodium crypto signature verify on Discord request failed, ".
                "bad signature $signature for public key $publicKey" );
        return false;
        }
    
    return true;
    }




function ds_showSteamLoginButton() {
    global $steamLoginURL, $serverRootURL, $fullServerURL, $steamButtonURL;

    // Nonce needed here?  No, because Steam's response contains a nonce
    $returnURL = $fullServerURL .
        "?action=steam_login_return";


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
    echo "<br><br>";
    
    }




// check if they own one of the games
// if so, either generate 4 secret words for them, or show them
// their existing four secret words
function ds_steamLoginReturn() {
    // php replaces all . with _ in URL parameter names!

    
    $signed = ds_requestFilter( "openid_signed", "/.*/" );
    $sig = ds_requestFilter( "openid_sig", "/.*/" );
    $ns = ds_requestFilter( "openid_ns", "/.*/" );
    $claimed_id = ds_requestFilter( "openid_claimed_id", "/.*/" );

    preg_match( "#https://steamcommunity.com/openid/id/(\d+)#",
                $claimed_id, $matches );
    
    $steam_id = $matches[1];
    
    
    
    // all other params that have been signed
    $signedList = explode( ",", $signed );
    
    
    $queryParams = array();

    foreach( $signedList as $paramName ) {
        $fullParamName = "openid_" . str_replace( ".", "_", $paramName );

        $paramValue = ds_requestFilter( "$fullParamName", "/.*/" );
        
        $queryParams["openid.$paramName"] = $paramValue;
        }

    $queryParams[ 'openid.mode' ] = "check_authentication";
    $queryParams[ 'openid.ns' ] = $ns;
    $queryParams[ 'openid.sig' ] = $sig;
    

    $formattedParams = http_build_query( $queryParams );
    
    
    // url to verify this with steam:

    global $steamLoginURL;
    
    
    $verifyURL = "$steamLoginURL?$formattedParams";

    $verifyResult = file_get_contents( $verifyURL );

    if( ! preg_match( "/is_valid\s*:\s*true/i", $verifyResult ) ) {
        echo "Failed to validate your Steam ID with Valve.<br><br>";
        return;
        }

    global $tableNamePrefix;
    

    // have a validated $steam_id


    // see if we already have a record for them
    
    $query = "SELECT discord_unlock_words FROM ".
        "$tableNamePrefix"."users ".
        "WHERE steam_id = '$steam_id';";

    $result = ds_queryDatabase( $query );
    
    $recordExists = ( mysqli_num_rows( $result ) == 1 );

    
    $discord_unlock_words = "";
    
    
    if( ! $recordExists ) {

        // fixme:
        // check if they own any game
        $ownOne = false;

        global $steamAppIDs;

        foreach( $steamAppIDs as $appID ) {

            if( ds_doesSteamUserOwnApp( $steam_id, $appID ) ) {
                $ownOne = true;
                break;
            }
        }

        if( ! $ownOne ) {

            echo "You don't own the necessary games on Steam.<br><br>";
            return;
        }

        $discord_unlock_words = ds_createAccount( $steam_id );
        }
    else {
        $discord_unlock_words = ds_mysqli_result( $result,
                                                  0, "discord_unlock_words" );
        }
    
    echo "<br><br>Here are your four secret words to join the ".
        "private Discord area:<br><br>";
    
    echo "$discord_unlock_words<br><br>";
    }



// returns true or false
function ds_doesSteamUserOwnApp( $inSteamID, $inAppID ) {
    global $steamWebAPIKey;
    
    $url = "https://partner.steam-api.com/ISteamUser/CheckAppOwnership/V0001".
        "?format=xml".
        "&key=$steamWebAPIKey".
        "&steamid=$inSteamID".
        "&appid=$inAppID";

    $result = file_get_contents( $url );

    
    $matched = preg_match( "#<ownsapp>(\w+)</ownsapp>#",
                           $result, $matches );

    if( $matched && $matches[1] == "true" ) {

        // make sure we return the true owner
        $matchedB = preg_match( "#<ownersteamid>(\d+)</ownersteamid>#",
                                $result, $matchesB );

        if( $matchedB && $matchesB[1] == $inSteamID ) {
            return true;
            }
        else {
            return false;
            }
        }
    else {
        return false;
        }
    }  





$ds_mysqlLink;


// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function ds_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName,
        $ds_mysqlLink;
    
    
    $ds_mysqlLink =
        mysqli_connect( $databaseServer, $databaseUsername, $databasePassword )
        or ds_operationError( "Could not connect to database server: " .
                              mysqli_error( $ds_mysqlLink ) );
    
    mysqli_select_db( $ds_mysqlLink, $databaseName )
        or ds_operationError( "Could not select $databaseName database: " .
                              mysqli_error( $ds_mysqlLink ) );
    }


 
/**
 * Closes the database connection.
 */
function ds_closeDatabase() {
    global $ds_mysqlLink;
    
    mysqli_close( $ds_mysqlLink );
    }


/**
 * Returns human-readable summary of a timespan.
 * Examples:  10.5 hours
 *            34 minutes
 *            45 seconds
 */
function ds_secondsToTimeSummary( $inSeconds ) {
    if( $inSeconds < 120 ) {
        if( $inSeconds == 1 ) {
            return "$inSeconds second";
            }
        return "$inSeconds seconds";
        }
    else if( $inSeconds < 3600 ) {
        $min = number_format( $inSeconds / 60, 0 );
        return "$min minutes";
        }
    else {
        $hours = number_format( $inSeconds / 3600, 1 );
        return "$hours hours";
        }
    }


/**
 * Returns human-readable summary of a distance back in time.
 * Examples:  10 hours
 *            34 minutes
 *            45 seconds
 *            19 days
 *            3 months
 *            2.5 years
 */
function ds_secondsToAgeSummary( $inSeconds ) {
    if( $inSeconds < 120 ) {
        if( $inSeconds == 1 ) {
            return "$inSeconds second";
            }
        return "$inSeconds seconds";
        }
    else if( $inSeconds < 3600 * 2 ) {
        $min = number_format( $inSeconds / 60, 0 );
        return "$min minutes";
        }
    else if( $inSeconds < 24 * 3600 * 2 ) {
        $hours = number_format( $inSeconds / 3600, 0 );
        return "$hours hours";
        }
    else if( $inSeconds < 24 * 3600 * 60 ) {
        $days = number_format( $inSeconds / ( 3600 * 24 ), 0 );
        return "$days days";
        }
    else if( $inSeconds < 24 * 3600 * 365 * 2 ) {
        // average number of days per month
        // based on 400 year calendar cycle
        // we skip a leap year every 100 years unless the year is divisible by 4
        $months = number_format( $inSeconds / ( 3600 * 24 * 30.436875 ), 0 );
        return "$months months";
        }
    else {
        // same logic behind computing average length of a year
        $years = number_format( $inSeconds / ( 3600 * 24 * 365.2425 ), 1 );
        return "$years years";
        }
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function ds_queryDatabase( $inQueryString ) {
    global $ds_mysqlLink;
    
    if( gettype( $ds_mysqlLink ) != "resource" ) {
        // not a valid mysql link?
        ds_connectToDatabase();
        }
    
    $result = mysqli_query( $ds_mysqlLink, $inQueryString );
    
    if( $result == FALSE ) {

        $errorNumber = mysqli_errno( $ds_mysqlLink );
        
        // server lost or gone?
        if( $errorNumber == 2006 ||
            $errorNumber == 2013 ||
            // access denied?
            $errorNumber == 1044 ||
            $errorNumber == 1045 ||
            // no db selected?
            $errorNumber == 1046 ) {

            // connect again?
            ds_closeDatabase();
            ds_connectToDatabase();

            $result = mysqli_query( $ds_mysqlLink, $inQueryString )
                or ds_operationError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysqli_error( $ds_mysqlLink ) );
            }
        else {
            // some other error (we're still connected, so we can
            // add log messages to database
            ds_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                           mysqli_error( $ds_mysqlLink ) );
            }
        }

    return $result;
    }



/**
 * Replacement for the old mysql_result function.
 */
function ds_mysqli_result( $result, $number, $field=0 ) {
    mysqli_data_seek( $result, $number );
    $row = mysqli_fetch_array( $result );
    return $row[ $field ];
    }



/**
 * Checks whether a table exists in the currently-connected database.
 *
 * @param $inTableName the name of the table to look for.
 *
 * @return 1 if the table exists, or 0 if not.
 */
function ds_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = ds_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = ds_mysqli_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function ds_log( $message ) {
    global $enableLog, $tableNamePrefix, $ds_mysqlLink;

    if( $enableLog ) {
        $slashedMessage = mysqli_real_escape_string( $ds_mysqlLink, $message );
    
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = ds_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function ds_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    ds_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function ds_operationError( $message ) {
    
    // for now, just print error message
    echo( "ERROR:  $message" );
    die();
    }


/**
 * Recursively applies the addslashes function to arrays of arrays.
 * This effectively forces magic_quote escaping behavior, eliminating
 * a slew of possible database security issues. 
 *
 * @inValue the value or array to addslashes to.
 *
 * @return the value or array with slashes added.
 */
function ds_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'ds_addslashes_deep', $inValue )
          : addslashes( $inValue ) );
    }



/**
 * Recursively applies the stripslashes function to arrays of arrays.
 * This effectively disables magic_quote escaping behavior. 
 *
 * @inValue the value or array to stripslashes from.
 *
 * @return the value or array with slashes removed.
 */
function ds_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'ds_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function ds_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
    if( ! isset( $_REQUEST[ $inRequestVariable ] ) ) {
        return $inDefault;
        }

    return ds_filter( $_REQUEST[ $inRequestVariable ], $inRegex, $inDefault );
    }


/**
 * Filters a value  using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function ds_filter( $inValue, $inRegex, $inDefault = "" ) {
    
    $numMatches = preg_match( $inRegex,
                              $inValue, $matches );

    if( $numMatches != 1 ) {
        return $inDefault;
        }
        
    return $matches[0];
    }



// this function checks the password directly from a request variable
// or via hash from a cookie.
//
// It then sets a new cookie for the next request.
//
// This avoids storing the password itself in the cookie, so a stale cookie
// (cached by a browser) can't be used to figure out the password and log in
// later. 
function ds_checkPassword( $inFunctionName ) {
    $password = "";
    $password_hash = "";

    $badCookie = false;
    
    
    global $accessPasswords, $tableNamePrefix, $remoteIP, $enableYubikey,
        $passwordHashingPepper;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    $passwordSent = false;
    
    if( isset( $_REQUEST[ "passwordHMAC" ] ) ) {
        $passwordHMAC = ds_requestFilter( "/[a-f0-9]+/i", "" );
        
        $passwordSent = true;

        // already hashed client-side on login form
        // hash again, because hash client sends us is not stored in
        // our settings file
        $password = ds_hmac_sha1( $passwordHashingPepper,
                                  $_REQUEST[ "passwordHMAC" ] );
        
        
        // generate a new hash cookie from this password
        $newSalt = time();
        $newHash = md5( $newSalt . $password );
        
        $password_hash = $newSalt . "_" . $newHash;
        }
    else if( isset( $_COOKIE[ $cookieName ] ) ) {
        ds_checkReferrer();
        $password_hash = ds_filter( $_COOKIE[ $cookieName ],
                                    "/[a-f0-9_]+/i", "" );
        
        // check that it's a good hash
        
        $hashParts = preg_split( "/_/", $password_hash );

        // default, to show in log message on failure
        // gets replaced if cookie contains a good hash
        $password = "(bad cookie:  $password_hash)";

        $badCookie = true;
        
        if( count( $hashParts ) == 2 ) {
            
            $salt = $hashParts[0];
            $hash = $hashParts[1];

            foreach( $accessPasswords as $truePassword ) {    
                $trueHash = md5( $salt . $truePassword );
            
                if( $trueHash == $hash ) {
                    $password = $truePassword;
                    $badCookie = false;
                    }
                }
            
            }
        }
    else {
        // no request variable, no cookie
        // cookie probably expired
        $badCookie = true;
        $password_hash = "(no cookie.  expired?)";
        }
    
        
    
    if( ! in_array( $password, $accessPasswords ) ) {

        if( ! $badCookie ) {
            
            echo "Incorrect password.";

            ds_log( "Failed $inFunctionName access with password:  ".
                    "$password" );
            }
        else {
            echo "Session expired.";
                
            ds_log( "Failed $inFunctionName access with bad cookie:  ".
                    "$password_hash" );
            }
        
        die();
        }
    else {
        
        if( $passwordSent && $enableYubikey ) {
            global $yubikeyIDs, $yubicoClientID, $yubicoSecretKey,
                $passwordHashingPepper;
            
            $yubikey = $_REQUEST[ "yubikey" ];

            $index = array_search( $password, $accessPasswords );
            $yubikeyIDList = preg_split( "/:/", $yubikeyIDs[ $index ] );

            $providedID = substr( $yubikey, 0, 12 );

            if( ! in_array( $providedID, $yubikeyIDList ) ) {
                echo "Provided Yubikey does not match ID for this password.";
                die();
                }
            
            
            $nonce = ds_hmac_sha1( $passwordHashingPepper, uniqid() );
            
            $callURL =
                "https://api2.yubico.com/wsapi/2.0/verify?id=$yubicoClientID".
                "&otp=$yubikey&nonce=$nonce";
            
            $result = trim( file_get_contents( $callURL ) );

            $resultLines = preg_split( "/\s+/", $result );

            sort( $resultLines );

            $resultPairs = array();

            $messageToSignParts = array();
            
            foreach( $resultLines as $line ) {
                // careful here, because = is used in base-64 encoding
                // replace first = in a line (the key/value separator)
                // with #
                
                $lineToParse = preg_replace( '/=/', '#', $line, 1 );

                // now split on # instead of =
                $parts = preg_split( "/#/", $lineToParse );

                $resultPairs[$parts[0]] = $parts[1];

                if( $parts[0] != "h" ) {
                    // include all but signature in message to sign
                    $messageToSignParts[] = $line;
                    }
                }
            $messageToSign = implode( "&", $messageToSignParts );

            $trueSig =
                base64_encode(
                    hash_hmac( 'sha1',
                               $messageToSign,
                               // need to pass in raw key
                               base64_decode( $yubicoSecretKey ),
                               true) );
            
            if( $trueSig != $resultPairs["h"] ) {
                echo "Yubikey authentication failed.<br>";
                echo "Bad signature from authentication server<br>";
                die();
                }

            $status = $resultPairs["status"];
            if( $status != "OK" ) {
                echo "Yubikey authentication failed: $status";
                die();
                }

            }
        
        // set cookie again, renewing it, expires in 24 hours
        $expireTime = time() + 60 * 60 * 24;
    
        setcookie( $cookieName, $password_hash, $expireTime, "/" );
        }
    }
 



function ds_clearPasswordCookie() {
    global $tableNamePrefix;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    // expire 24 hours ago (to avoid timezone issues)
    $expireTime = time() - 60 * 60 * 24;

    setcookie( $cookieName, "", $expireTime, "/" );
    }
 
 







function ds_hmac_sha1( $inKey, $inData ) {
    return hash_hmac( "sha1", 
                      $inData, $inKey );
    } 

 
function ds_hmac_sha1_raw( $inKey, $inData ) {
    return hash_hmac( "sha1", 
                      $inData, $inKey, true );
    } 



function ds_hmac_sha256( $inKey, $inData ) {
    return hash_hmac( "sha256", 
                      $inData, $inKey );
    } 

 
 
 
 
// decodes a ASCII hex string into an array of 0s and 1s 
function ds_hexDecodeToBitString( $inHexString ) {
    $digits = str_split( $inHexString );

    $bitString = "";

    foreach( $digits as $digit ) {
        $index = hexdec( $digit );

        $binDigitString = decbin( $index );

        // pad with 0s
        $binDigitString =
            substr( "0000", 0, 4 - strlen( $binDigitString ) ) .
            $binDigitString;

        $bitString = $bitString . $binDigitString;
        }

    return $bitString;
    }
 


function ds_arrayRemoveByValue( &$inArray, $inValue ) {
    if( ( $key = array_search( $inValue, $inArray ) ) !== false ) {
        unset( $inArray[$key] );
        }
    }





function ds_mail( $inEmail,
                  $inSubject,
                  $inBody,
                  // true for transactional emails that should use
                  // a different SMTP
                  $inTrans = false ) {
    
    global $useSMTP, $siteEmailAddress, $siteEmailDomain;

    if( $useSMTP ) {
        require_once "Mail.php";

        global $smtpHost, $smtpPort, $smtpUsername, $smtpPassword;

        $messageID = "<" . uniqid() . "@$siteEmailDomain>";
        
        $headers = array( 'From' => $siteEmailAddress,
                          'To' => $inEmail,
                          'Subject' => $inSubject,
                          'Date' => date( "r" ),
                          'Message-Id' => $messageID );
        $smtp;

        if( $inTrans ) {
            global $smtpHostTrans, $smtpPortTrans,
                $smtpUsernameTrans, $smtpPasswordTrans;

            $smtp = Mail::factory( 'smtp',
                                   array ( 'host' => $smtpHostTrans,
                                           'port' => $smtpPortTrans,
                                           'auth' => true,
                                           'username' => $smtpUsernameTrans,
                                           'password' => $smtpPasswordTrans ) );
            }
        else {
            $smtp = Mail::factory( 'smtp',
                                   array ( 'host' => $smtpHost,
                                           'port' => $smtpPort,
                                           'auth' => true,
                                           'username' => $smtpUsername,
                                           'password' => $smtpPassword ) );
            }
        

        $mail = $smtp->send( $inEmail, $headers, $inBody );


        if( PEAR::isError( $mail ) ) {
            ds_log( "Email send failed ($inEmail):  " .
                    $mail->getMessage() );
            return false;
            }
        else {
            return true;
            }
        }
    else {
        // raw sendmail
        $mailHeaders = "From: $siteEmailAddress";
        
        return mail( $inEmail,
                     $inSubject,
                     $inBody,
                     $mailHeaders );
        }
    }


 
?>
