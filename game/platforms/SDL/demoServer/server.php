<?php



global $dps_version;
$dps_version = "1";



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
<HEAD><TITLE>Demo Permissions Server Web-based setup</TITLE></HEAD>
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






// ensure that magic quotes are on (adding slashes before quotes
// so that user-submitted data can be safely submitted in DB queries)
if( !get_magic_quotes_gpc() ) {
    // force magic quotes to be added
    $_GET     = array_map( 'dps_addslashes_deep', $_GET );
    $_POST    = array_map( 'dps_addslashes_deep', $_POST );
    $_REQUEST = array_map( 'dps_addslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'dps_addslashes_deep', $_COOKIE );
    }
    





// all calls need to connect to DB, so do it once here
dps_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = "";
if( isset( $_REQUEST[ "action" ] ) ) {
    $action = $_REQUEST[ "action" ];
    }

$debug = "";
if( isset( $_REQUEST[ "debug" ] ) ) {
    $debug = $_REQUEST[ "debug" ];
    }

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }




if( $action == "version" ) {
    global $dps_version;
    echo "$dps_version";
    }
else if( $action == "show_log" ) {
    dps_showLog();
    }
else if( $action == "clear_log" ) {
    dps_clearLog();
    }
else if( $action == "create_demo_id" ) {
    dps_createDemoID();
    }
else if( $action == "block_demo_id" ) {
    dps_blockDemoID();
    }
else if( $action == "delete_demo_id" ) {
    dps_deleteDemoID();
    }
else if( $action == "check_permitted" ) {
    dps_checkPermitted();
    }
else if( $action == "show_data" ) {
    dps_showData();
    }
else if( $action == "show_detail" ) {
    dps_showDetail();
    }
else if( $action == "dps_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Demo Permissions Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    dps_setupDatabase();

    echo "</TD></TR></TABLE></TD></TR></TABLE></CENTER><BR><BR>";
    
    echo $setup_footer;
    }
else if( preg_match( "/server\.php/", $_SERVER[ "SCRIPT_NAME" ] ) ) {
    // server.php has been called without an action parameter

    // the preg_match ensures that server.php was called directly and
    // not just included by another script
    
    // quick (and incomplete) test to see if we should show instructions
    global $tableNamePrefix;
    
    // check if our "games" table exists
    $tableName = $tableNamePrefix . "demos";
    
    $exists = dps_doesTableExist( $tableName );
        
    if( $exists  ) {
        echo "Demo Permissions server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Demo Permissions Server Web-based Setup</H2>";
    
        echo "Demo Permissions Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=dps_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

dps_closeDatabase();







/**
 * Creates the database tables needed by seedBlogs.
 */
function dps_setupDatabase() {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . "log";
    if( ! dps_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = dps_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "demos";
    if( ! dps_doesTableExist( $tableName ) ) {

        // this table contains general info about each game
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "demo_id CHAR(10) NOT NULL PRIMARY KEY," .
            "creation_date DATETIME NOT NULL," .
            "last_run_date DATETIME NOT NULL," .
            "note CHAR(40) NOT NULL," .
            "blocked TINYINT NOT NULL," .
            "run_count INT NOT NULL );";

        $result = dps_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    
    $tableName = $tableNamePrefix . "runs";
    if( ! dps_doesTableExist( $tableName ) ) {

        // this table contains information for each user
        $query =
            "CREATE TABLE $tableName(" .
            "demo_id CHAR(10) NOT NULL," .
            "run_date DATETIME NOT NULL," .
            "blocked TINYINT NOT NULL," .
            "ip_address CHAR(255) NOT NULL," .
            "PRIMARY KEY( demo_id, run_date ) );";
                
        $result = dps_queryDatabase( $query );
        
        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }



function dps_showLog() {
    $password = dps_checkPassword( "show_log" );

     echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";
    $result = dps_queryDatabase( $query );

    $numRows = mysql_numrows( $result );



    echo "<a href=\"server.php?action=clear_log&password=$password\">".
        "Clear log</a>";
        
    echo "<hr>";
        
    echo "$numRows log entries:<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $time = mysql_result( $result, $i, "entry_time" );
        $entry = mysql_result( $result, $i, "entry" );

        echo "<b>$time</b>:<br>$entry<hr>\n";
        }
    }



function dps_clearLog() {
    $password = dps_checkPassword( "clear_log" );

     echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br>";
    
    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = dps_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }







function dps_createDemoID() {
    $password = dps_checkPassword( "create_demo_id" );

    global $tableNamePrefix;
    
    
    $note = "";
    if( isset( $_REQUEST[ "note" ] ) ) {
        $note = $_REQUEST[ "note" ];
        }


    
    $found_unused_id = 0;
    $salt = 0;
    
    
    while( ! $found_unused_id ) {

        $randVal = rand();
        
        $hash = md5( $note . uniqid( "$randVal"."$salt", true ) );

        $hash = strtoupper( $hash );
        
        
        $demo_id = substr( $hash, 0, 10 );


        
        // make code more human-friendly (alpha only)

        $digitArray =
            array( "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" );
        $letterArray =
            array( "W", "H", "J", "K", "X", "M", "N", "P", "T", "Y" );

        $demo_id = str_replace( $digitArray, $letterArray, $demo_id );
        


        $query = "INSERT INTO $tableNamePrefix". "demos VALUES ( " .
            "'$demo_id', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, ".
            "'$note', '0', '0' );";


        $result = mysql_query( $query );

        if( $result ) {
            $found_unused_id = 1;

            global $remoteIP;
            dps_log( "Demo $demo_id created by $remoteIP" );

            
            //echo "$demo_id";
            dps_showData();
            }
        else {
            global $debug;
            if( $debug == 1 ) {
                echo "Duplicate ids?  Error:  " . mysql_error() ."<br>";
                }
            // try again
            $salt += 1;
            }
        }

    }



function dps_blockDemoID() {
    $password = dps_checkPassword( "block_demo_id" );


    global $tableNamePrefix;
        
    $demo_id = "";
    if( isset( $_REQUEST[ "demo_id" ] ) ) {
        $demo_id = $_REQUEST[ "demo_id" ];
        }

    $demo_id = strtoupper( $demo_id );
    

    $blocked = "";
    if( isset( $_REQUEST[ "blocked" ] ) ) {
        $blocked = $_REQUEST[ "blocked" ];
        }

    
    global $remoteIP;

    

    
    $query = "SELECT * FROM $tableNamePrefix"."demos ".
        "WHERE demo_id = '$demo_id';";
    $result = dps_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {

        
        $query = "UPDATE $tableNamePrefix"."demos SET " .
            "blocked = '$blocked' " .
            "WHERE demo_id = '$demo_id';";
        
        $result = dps_queryDatabase( $query );

        
        dps_log( "$demo_id block changed to $blocked by $remoteIP" );

        dps_showData();
        }
    else {
        dps_log( "$demo_id not found for $remoteIP" );

        echo "$demo_id not found";
        }    
    }



function dps_deleteDemoID() {
    $password = dps_checkPassword( "delete_demo_id" );

    global $tableNamePrefix, $remoteIP;
        
    $demo_id = "";
    if( isset( $_REQUEST[ "demo_id" ] ) ) {
        $demo_id = $_REQUEST[ "demo_id" ];
        }

    $demo_id = strtoupper( $demo_id );
    

    $query = "DELETE FROM $tableNamePrefix"."demos ".
        "WHERE demo_id = '$demo_id';";
    $result = dps_queryDatabase( $query );
    
    if( $result ) {
        dps_log( "$demo_id deleted by $remoteIP" );

        echo "$demo_id deleted.<hr>";
        
        dps_showData();
        }
    else {
        dps_log( "$demo_id delete failed for $remoteIP" );

        echo "DELETE operation failed?";
        }
    }



function dps_checkPermitted() {
    $demo_id = "";
    if( isset( $_REQUEST[ "demo_id" ] ) ) {
        $demo_id = $_REQUEST[ "demo_id" ];
        }

    $demo_id = strtoupper( $demo_id );
    

    $challenge = "";
    if( isset( $_REQUEST[ "challenge" ] ) ) {
        $challenge = $_REQUEST[ "challenge" ];
        }
    
    
    global $tableNamePrefix, $remoteIP;

    

    
    $query = "SELECT * FROM $tableNamePrefix"."demos ".
        "WHERE demo_id = '$demo_id';";
    $result = dps_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );

        $blocked = $row[ "blocked" ];


        // catalog blocked runs, too
        $run_count = $row[ "run_count" ];
        
        $run_count ++;
        
        
        $query = "UPDATE $tableNamePrefix"."demos SET " .
            "last_run_date = CURRENT_TIMESTAMP, " .
            "run_count = '$run_count' " .
            "WHERE demo_id = '$demo_id';";
            

        $result = dps_queryDatabase( $query );
        
        $query = "INSERT INTO $tableNamePrefix". "runs VALUES ( " .
            "'$demo_id', CURRENT_TIMESTAMP, '$blocked', '$remoteIP' );";
        
        $result = mysql_query( $query );

        if( !$blocked ) {
            
            dps_log( "$demo_id permitted to run by $remoteIP" );
        

            // response to challenge using shared secret
            
            global $sharedSecret;
            
            $hash = sha1( $challenge . $sharedSecret );

            $hash = strtoupper( $hash );
            

            
            echo "permitted $hash";
            return;
            }
        }

    dps_log( "$demo_id denied to run by $remoteIP" );

    echo "denied";
    }





function dps_showData() {
    $password = dps_checkPassword( "show_data" );

    global $tableNamePrefix, $remoteIP;
    

    echo "[<a href=\"server.php?action=show_data&password=$password" .
            "\">Main</a>]";
    
    
    $query = "SELECT * FROM $tableNamePrefix"."demos ".
        "ORDER BY last_run_date DESC;";
    $result = dps_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    // form
        ?>    

        <hr>
        Create new ID:<br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="create_demo_id">
    Note:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="note">
            <INPUT TYPE="Submit" VALUE="Generate">
    </FORM>
        <hr>

        <?php

    
    echo "$numRows active IDs:<br><br><br>\n";

    echo "<table border=1 cellpadding=5>\n";

    echo "<tr><td>Demo ID</td>\n";
    echo "<td>Note</td>\n";
    echo "<td>Blocked?</td>\n";
    echo "<td>Created</td> <td>Test</td> <td>Last Run</td>";
    echo "<td>Run Count</td></tr>\n";


    for( $i=0; $i<$numRows; $i++ ) {
        $demo_id = mysql_result( $result, $i, "demo_id" );
        $creation = mysql_result( $result, $i, "creation_date" );
        $lastRun = mysql_result( $result, $i, "last_run_date" );
        $count = mysql_result( $result, $i, "run_count" );
        $note = mysql_result( $result, $i, "note" );
        $blocked = mysql_result( $result, $i, "blocked" );

        $block_toggle = "";
        
        if( $blocked ) {
            $blocked = "BLOCKED";
            $block_toggle = "<a href=\"server.php?action=block_demo_id&".
                "blocked=0&demo_id=$demo_id&password=$password\">unblock</a>";
            
            }
        else {
            $blocked = "";
            $block_toggle = "<a href=\"server.php?action=block_demo_id&".
                "blocked=1&demo_id=$demo_id&password=$password\">block</a>";
            
            }


        // challenge to include in test link
        $randVal = rand();
        
        $challenge = md5( $demo_id . uniqid( "$randVal", true ) );
        
        
        echo "<tr>\n";
        
        echo "<td><b>$demo_id</b></td>\n";
        echo "<td>$note</td>\n";
        echo "<td align=right>$blocked [$block_toggle]</td>\n";
        echo "<td>$creation</td> ";
        echo "<td>[<a href=\"server.php?action=check_permitted".
            "&demo_id=$demo_id&challenge=$challenge\">run test</a>]</td>";
        echo "<td>$lastRun</td>";
        echo "<td>$count runs ";

        echo "[<a href=\"server.php?action=show_detail&password=$password" .
            "&demo_id=$demo_id\">list</a>]</td>";
        
        echo "</tr>\n";
        }
    echo "</table>";
    echo "<hr>";

    echo "<a href=\"server.php?action=show_log&password=$password\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    }



function dps_showDetail() {
    $password = dps_checkPassword( "show_detail" );

     echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]";
    
    global $tableNamePrefix;
    

    $demo_id = "";
    if( isset( $_REQUEST[ "demo_id" ] ) ) {
        $demo_id = $_REQUEST[ "demo_id" ];
        }

    $demo_id = strtoupper( $demo_id );

    
    $query = "SELECT * FROM $tableNamePrefix"."runs ".
        "WHERE demo_id = '$demo_id' ORDER BY run_date DESC;";
    $result = dps_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    echo "$numRows runs for $demo_id:";

    echo " [<a href=\"server.php?action=delete_demo_id&password=$password" .
        "&demo_id=$demo_id\">DELETE this id</a>]";
    
    echo "<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $date = mysql_result( $result, $i, "run_date" );
        $ipAddress = mysql_result( $result, $i, "ip_address" );

        $blocked = mysql_result( $result, $i, "blocked" );

        if( $blocked ) {
            $blocked = "BLOCKED";
            }
        else {
            $blocked = "";
            }
        
        echo "<b>$date</b>: $ipAddress $blocked<hr>\n";
        }
    }






// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function dps_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName;
    
    
    mysql_connect( $databaseServer, $databaseUsername, $databasePassword )
        or dps_fatalError( "Could not connect to database server: " .
                       mysql_error() );
    
	mysql_select_db( $databaseName )
        or dps_fatalError( "Could not select $databaseName database: " .
                       mysql_error() );
    }


 
/**
 * Closes the database connection.
 */
function dps_closeDatabase() {
    mysql_close();
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function dps_queryDatabase( $inQueryString ) {

    $result = mysql_query( $inQueryString )
        or dps_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                       mysql_error() );

    return $result;
    }



/**
 * Checks whether a table exists in the currently-connected database.
 *
 * @param $inTableName the name of the table to look for.
 *
 * @return 1 if the table exists, or 0 if not.
 */
function dps_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = dps_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = mysql_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function dps_log( $message ) {
    global $enableLog, $tableNamePrefix;

    $slashedMessage = addslashes( $message );
    
    if( $enableLog ) {
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = dps_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function dps_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    dps_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function dps_operationError( $message ) {
    
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
function dps_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'dps_addslashes_deep', $inValue )
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
function dps_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'sb_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



function dps_checkPassword( $inFunctionName ) {
    $password = "";
    if( isset( $_REQUEST[ "password" ] ) ) {
        $password = $_REQUEST[ "password" ];
        }

    global $accessPassword, $tableNamePrefix, $remoteIP;
    
    if( $password != $accessPassword ) {
        echo "Incorrect password.";

        dps_log( "Failed $inFunctionName access with password:  $password" );

        die();
        }

    return $password;
    }


?>
