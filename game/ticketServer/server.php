<?php



global $ts_version;
$ts_version = "1";



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
<HEAD><TITLE>Ticket Permissions Server Web-based setup</TITLE></HEAD>
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
    $_GET     = array_map( 'ts_addslashes_deep', $_GET );
    $_POST    = array_map( 'ts_addslashes_deep', $_POST );
    $_REQUEST = array_map( 'ts_addslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'ts_addslashes_deep', $_COOKIE );
    }
    





// all calls need to connect to DB, so do it once here
ts_connectToDatabase();

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
    global $ts_version;
    echo "$ts_version";
    }
else if( $action == "show_log" ) {
    ts_showLog();
    }
else if( $action == "clear_log" ) {
    ts_clearLog();
    }
else if( $action == "sell_ticket" ) {
    ts_sellTicket();
    }
else if( $action == "block_ticket_id" ) {
    ts_blockTicketID();
    }
else if( $action == "delete_ticket_id" ) {
    ts_deleteTicketID();
    }
else if( $action == "show_downloads" ) {
    ts_showDownloads();
    }
else if( $action == "download" ) {
    ts_download();
    }
else if( $action == "show_data" ) {
    ts_showData();
    }
else if( $action == "show_detail" ) {
    ts_showDetail();
    }
else if( $action == "edit_ticket" ) {
    ts_editTicket();
    }
else if( $action == "send_group_email" ) {
    ts_sendGroupEmail();
    }
else if( $action == "send_single_email" ) {
    ts_sendSingleEmail();
    }
else if( $action == "send_all_note" ) {
    ts_sendAllNote();
    }
else if( $action == "send_all_file_note" ) {
    ts_sendAllFileNote();
    }
else if( $action == "email_opt_in" ) {
    ts_emailOptIn();
    }
else if( $action == "ts_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Ticket Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    ts_setupDatabase();

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
    $tableName = $tableNamePrefix . "tickets";
    
    $exists = ts_doesTableExist( $tableName );
        
    if( $exists  ) {
        echo "Ticket Server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Ticket Server Web-based Setup</H2>";
    
        echo "Ticket Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=ts_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

ts_closeDatabase();







/**
 * Creates the database tables needed by seedBlogs.
 */
function ts_setupDatabase() {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . "log";
    if( ! ts_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = ts_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "tickets";
    if( ! ts_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "ticket_id CHAR(10) NOT NULL PRIMARY KEY," .
            "sale_date DATETIME NOT NULL," .
            "last_download_date DATETIME NOT NULL," .
            "name TEXT NOT NULL, ".
            "email CHAR(255) NOT NULL," .
            "order_number CHAR(255) NOT NULL," .
            "tag CHAR(255) NOT NULL," .
            "email_sent TINYINT NOT NULL," .
            "blocked TINYINT NOT NULL," .
            "download_count INT NOT NULL, ".
            "email_opt_in TINYINT NOT NULL );";

        $result = ts_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }


    
    
    $tableName = $tableNamePrefix . "downloads";
    if( ! ts_doesTableExist( $tableName ) ) {

        // this table contains information about each download that occurred
        $query =
            "CREATE TABLE $tableName(" .
            "ticket_id CHAR(10) NOT NULL," .
            "download_date DATETIME NOT NULL," .
            "file_name TEXT NOT NULL," .
            "blocked TINYINT NOT NULL," .
            "ip_address CHAR(255) NOT NULL," .
            "PRIMARY KEY( ticket_id, download_date ) );";
                
        $result = ts_queryDatabase( $query );
        
        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }



function ts_showLog() {
    $password = ts_checkPassword( "show_log" );

     echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";
    $result = ts_queryDatabase( $query );

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



function ts_clearLog() {
    $password = ts_checkPassword( "clear_log" );

     echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = ts_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }







function ts_sellTicket() {
    global $tableNamePrefix, $fastspringPrivateKeys, $remoteIP;


    $tag = "";
    if( isset( $_REQUEST[ "tags" ] ) ) {
        $tag = $_REQUEST[ "tags" ];
        }

    $privateKey = $fastspringPrivateKeys[ $tag ];
    

    
    $security_data = $_REQUEST[ "security_data" ];
    $security_hash = $_REQUEST[ "security_hash" ];

    $string_to_hash = $security_data . $privateKey;
    
    $correct_hash = md5( $string_to_hash );
    

    if( $correct_hash != $security_hash ) {

        
        ts_log( "Ticket sale security check failed, from $remoteIP, ".
                "data = \"$security_data\", hash = \"$security_hash\",".
                "looking for hash = \"$correct_hash\"," .
                "(data hashed = \"$string_to_hash\")" );
        
        return;  /* FAILED CHECK */
        }

    
    $name = "";
    if( isset( $_REQUEST[ "name" ] ) ) {
        $name = $_REQUEST[ "name" ];
        }

    $email = "";
    if( isset( $_REQUEST[ "email" ] ) ) {
        $email = $_REQUEST[ "email" ];
        }

    $order_number = "";
    if( isset( $_REQUEST[ "reference" ] ) ) {
        $order_number = $_REQUEST[ "reference" ];
        }



    
    $found_unused_id = 0;
    $salt = 0;
    
    
    while( ! $found_unused_id ) {

        $randVal = rand();
        
        $hash = md5( $name . uniqid( "$randVal"."$salt", true ) );

        $hash = strtoupper( $hash );
        
        
        $ticket_id = substr( $hash, 0, 10 );


        
        // make code more human-friendly (alpha only)

        $digitArray =
            array( "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" );
        $letterArray =
            array( "W", "H", "J", "K", "X", "M", "N", "P", "T", "Y" );

        $ticket_id = str_replace( $digitArray, $letterArray, $ticket_id );


        /*
"ticket_id CHAR(10) NOT NULL PRIMARY KEY," .
            "sale_date DATETIME NOT NULL," .
            "last_download_date DATETIME NOT NULL," .
            "name TEXT NOT NULL, ".
            "email CHAR(255) NOT NULL," .
            "order_number CHAR(255) NOT NULL," .
            "tag CHAR(255) NOT NULL," .
            "blocked TINYINT NOT NULL," .
            "download_count INT
         */


        $query = "INSERT INTO $tableNamePrefix". "tickets VALUES ( " .
            "'$ticket_id', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, ".
            "'$name', '$email', '$order_number', '$tag', '0', '0', '0', '0' );";


        $result = mysql_query( $query );

        if( $result ) {
            $found_unused_id = 1;

            ts_log( "Ticket $ticket_id created by $remoteIP" );

            
            echo "$ticket_id";
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




function ts_editTicket() {

    $password = ts_checkPassword( "edit_ticket" );
    global $tableNamePrefix, $remoteIP;


    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    
    $name = "";
    if( isset( $_REQUEST[ "name" ] ) ) {
        $name = $_REQUEST[ "name" ];
        }

    $email = "";
    if( isset( $_REQUEST[ "email" ] ) ) {
        $email = $_REQUEST[ "email" ];
        }

    $order_number = "";
    if( isset( $_REQUEST[ "reference" ] ) ) {
        $order_number = $_REQUEST[ "reference" ];
        }

    $tag = "";
    if( isset( $_REQUEST[ "tag" ] ) ) {
        $tag = $_REQUEST[ "tag" ];
        }



    $query = "UPDATE $tableNamePrefix". "tickets SET " .
        "name = '$name', email = '$email', ".
        "order_number = '$order_number', tag = '$tag' ".
        "WHERE ticket_id = '$ticket_id';";
    

    $result = mysql_query( $query );

    if( $result ) {
        ts_log( "$ticket_id data changed by $remoteIP" );
        echo "Update of $ticket_id succeeded<br><br>";
        
        ts_showDetail();
        }
    else {
        ts_log( "$ticket_id data change failed for $remoteIP" );

        echo "Update of $ticket_id failed";
        }
    }



function ts_blockTicketID() {
    $password = ts_checkPassword( "block_ticket_id" );


    global $tableNamePrefix;
        
    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );
    

    $blocked = "";
    if( isset( $_REQUEST[ "blocked" ] ) ) {
        $blocked = $_REQUEST[ "blocked" ];
        }

    
    global $remoteIP;

    

    
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";
    $result = ts_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {

        
        $query = "UPDATE $tableNamePrefix"."tickets SET " .
            "blocked = '$blocked' " .
            "WHERE ticket_id = '$ticket_id';";
        
        $result = ts_queryDatabase( $query );

        
        ts_log( "$ticket_id block changed to $blocked by $remoteIP" );

        ts_showData();
        }
    else {
        ts_log( "$ticket_id not found for $remoteIP" );

        echo "$ticket_id not found";
        }    
    }



function ts_deleteTicketID() {
    $password = ts_checkPassword( "delete_ticket_id" );

    global $tableNamePrefix, $remoteIP;
        
    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );
    

    $query = "DELETE FROM $tableNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";
    $result = ts_queryDatabase( $query );
    
    if( $result ) {
        ts_log( "$ticket_id deleted by $remoteIP" );

        echo "$ticket_id deleted.<hr>";
        
        ts_showData();
        }
    else {
        ts_log( "$ticket_id delete failed for $remoteIP" );

        echo "DELETE operation failed?";
        }
    }



function ts_downloadAllowed() {

    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );    
    
    global $tableNamePrefix, $remoteIP;


    global $header, $footer;


    
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";
    $result = ts_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );

        $blocked = $row[ "blocked" ];

        $tag = $row[ "tag" ];



        date_default_timezone_set( "America/New_York" );
        

        $currentTimestamp = time();

        $allowedTimestamp;

        $downloadReady = 1;
        
        
        global $allowedDownloadDates;
        
        $allowedTimestamp = strtotime( $allowedDownloadDates[ $tag ] );


        
        if( $currentTimestamp < $allowedTimestamp ) {

            eval( $header );
            
            $allowedDateString = date( "l, F j, Y", $allowedTimestamp );
            echo "Your download will be available on ".
                "<b>$allowedDateString</b> (New York Time)<br>\n";

            $d = $allowedTimestamp - $currentTimestamp;

            $hours = (int)( $d / 3600 );

            $seconds = (int)( $d % 3600 );
            $minutes = (int)( $seconds / 60 );
            $seconds = (int)( $seconds % 60 );
            
            $days = (int)( $hours / 24 );
            $hours = (int)( $hours % 24 );
            

            echo "(That's in $days days, $hours hours, ".
                "$minutes minutes, and $seconds seconds)<br>\n";

            $currentDateString = date( "l, F j, Y [g:i a]",
                                       $currentTimestamp );

            echo "Current New York time: $currentDateString<br>\n";

            eval( $footer );
            
            $downloadReady = 0;
            }
        
            
        
        // format as in    Sunday, July 7, 2005 [4:52 pm]
        //$dateString = date( "l, F j, Y [g:i a]", $timestamp );

        if( !$blocked ){
            $blocked = !$downloadReady;
            }

        if( !$blocked ) {
            
            ts_log( "$ticket_id permitted to download by $remoteIP" );
            
            return 1;
            }
        else {
            
            if( $downloadReady ) {
                eval( $header );
                echo "Your download access is currently blocked";
                eval( $footer );

                ts_log( "$ticket_id denied to download by ".
                        "$remoteIP (blocked)" );
                }
            else {
                ts_log( "$ticket_id denied to download by ".
                        "$remoteIP (too early)" );
                }
            
            return 0;
            }
        }
    eval( $header );
    echo "Your ticket number was not found";
    eval( $footer );
    
    ts_log( "$ticket_id denied to download by $remoteIP (not found)" );

    return 0;
    }


function ts_printLink( $inFileName, $inTicketID ) {
    echo "<a href=\"server.php?action=download&ticket_id=$inTicketID&" .
        "file_name=$inFileName\">$inFileName</a>";
    }



function ts_showDownloads() {
    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );    
    
    global $tableNamePrefix, $remoteIP;

    
    if( ts_downloadAllowed() ) {
        global $fileList, $fileDescriptions, $fileListHeader, $footer;


        eval( $fileListHeader );
        
        echo "<center><table border=1 cellpadding=4>";
        
        for( $i=0; $i<count( $fileList ); $i++ ) {
            echo "<tr><td>";
            ts_printLink( $fileList[$i], $ticket_id );
            echo "</td>";
            $des = $fileDescriptions[$i];
            echo "<td>$des</td></tr>";
            }
        echo "</table></center>";



        // show opt in or opt out link?
        $query = "SELECT * FROM $tableNamePrefix"."tickets ".
            "WHERE ticket_id = '$ticket_id';";
        $result = ts_queryDatabase( $query );

        $numRows = mysql_numrows( $result );

        if( $numRows == 1 ) {
        
            $row = mysql_fetch_array( $result, MYSQL_ASSOC );

            $email_opt_in = $row[ "email_opt_in" ];

            echo "<br><br>";
            
            if( $email_opt_in == '1' ) {
                echo "[<a href=\"server.php?action=email_opt_in&in=0&".
                    "ticket_id=$ticket_id\">Opt Out</a>] of email updates.";
                }
            else {
                echo "[<a href=\"server.php?action=email_opt_in&in=1&".
                    "ticket_id=$ticket_id\">Opt In</a>] to email updates.";
                }
            }
        
        
        eval( $footer );
        }
    }




function ts_download() {
    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );    

    $file_name = "";
    if( isset( $_REQUEST[ "file_name" ] ) ) {
        $file_name = $_REQUEST[ "file_name" ];
        }

    
    global $tableNamePrefix, $remoteIP;

    
        
    $blocked = ! ts_downloadAllowed();
    

    
    
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";
    $result = ts_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );


        // catalog blocked runs, too
        $download_count = $row[ "download_count" ];
        
        $download_count ++;
        
        
        $query = "UPDATE $tableNamePrefix"."tickets SET " .
            "last_download_date = CURRENT_TIMESTAMP, " .
            "download_count = '$download_count' " .
            "WHERE ticket_id = '$ticket_id';";
            

        $result = ts_queryDatabase( $query );
        
        
        $query = "INSERT INTO $tableNamePrefix". "downloads VALUES ( " .
            "'$ticket_id', CURRENT_TIMESTAMP, '$file_name', ".
            "'$blocked', '$remoteIP' );";
        
        $result = mysql_query( $query );

        
        if( !$blocked ) {
            global $downloadFilePath;
            
            $result = ts_send_file( $downloadFilePath . $file_name );

            if( ! $result ) {
                global $header, $footer;

                eval( $header );
                
                echo "File not found.";
                
                eval( $footer );
                }
            
            return;
            }
        else {
            return;
            }
        }
    }




function ts_emailOptIn() {
    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );    

    $in = "";
    if( isset( $_REQUEST[ "in" ] ) ) {
        $in = $_REQUEST[ "in" ];
        }

    
    global $tableNamePrefix, $remoteIP;

    
    if( ts_downloadAllowed() ) {
        global $header, $footer;


        eval( $header );


        $query = "UPDATE $tableNamePrefix"."tickets ".
            "SET email_opt_in='$in' WHERE ticket_id = '$ticket_id';";

        $result = ts_queryDatabase( $query );

        echo "Email updates for your download ticket are currently ";

        if( $in == 1 ) {
            echo "<b>on</b><br><br>\n";
            }
        else {
            echo "<b>off</b><br><br>\n";
            }
        

        // show opt in or opt out link?
        $query = "SELECT * FROM $tableNamePrefix"."tickets ".
            "WHERE ticket_id = '$ticket_id';";
        $result = ts_queryDatabase( $query );

        $numRows = mysql_numrows( $result );

        if( $numRows == 1 ) {
        
            $row = mysql_fetch_array( $result, MYSQL_ASSOC );

            $email_opt_in = $row[ "email_opt_in" ];

            
            
            if( $email_opt_in == '1' ) {
                echo "[<a href=\"server.php?action=email_opt_in&in=0&".
                    "ticket_id=$ticket_id\">Opt Out</a>] of email updates.";
                }
            else {
                echo "[<a href=\"server.php?action=email_opt_in&in=1&".
                    "ticket_id=$ticket_id\">Opt In</a>] to email updates.";
                }
            }
        
        
        eval( $footer );
        }
    }





function ts_showData() {
    $password = ts_checkPassword( "show_data" );

    global $tableNamePrefix, $remoteIP;
    

    echo "[<a href=\"server.php?action=show_data&password=$password" .
            "\">Main</a>]<br><br><br>";




    $skip = 0;
    if( isset( $_REQUEST[ "skip" ] ) ) {
        $skip = $_REQUEST[ "skip" ];
        }

    global $ticketsPerPage;
    
    $search = "";
    if( isset( $_REQUEST[ "search" ] ) ) {
        $search = $_REQUEST[ "search" ];
        }

    $keywordClause = "";
    $searchDisplay = "";
    
    if( $search != "" ) {
        

        $keywordClause = "WHERE ( name LIKE '%$search%' " .
            "OR email LIKE '%$search%' ".
            "OR ticket_id LIKE '%$search%' ".
            "OR tag LIKE '%$search%' ) ";

        $searchDisplay = " matching <b>$search</b>";
        }
    

    

    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."tickets $keywordClause;";

    $result = ts_queryDatabase( $query );
    $totalTickets = mysql_result( $result, 0, 0 );

    
             
    $query = "SELECT * FROM $tableNamePrefix"."tickets $keywordClause".
        "ORDER BY last_download_date DESC ".
        "LIMIT $skip, $ticketsPerPage;";
    $result = ts_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $ticketsPerPage - 1;

    if( $endSkip > $totalTickets ) {
        $endSkip = $totalTickets;
        }



        // form for searching tickets
?>
        <hr>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="show_data">
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="search"
             VALUE="<?php echo $search;?>">
    <INPUT TYPE="Submit" VALUE="Search">
    </FORM>
        <hr>
<?php

    

    
    echo "$totalTickets active tickets". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $ticketsPerPage;

    $prevSkip = $skip - $ticketsPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data&password=$password" .
            "&skip=$prevSkip&search=$search\">Previous Page</a>] ";
        }
    if( $nextSkip < $totalTickets ) {
        echo "[<a href=\"server.php?action=show_data&password=$password" .
            "&skip=$nextSkip&search=$search\">Next Page</a>]";
        }

    echo "<br><br>";
    
    echo "<table border=1 cellpadding=5>\n";

    echo "<tr><td>Ticket ID</td>\n";
    echo "<td>Name</td>\n";
    echo "<td>Email</td>\n";
    echo "<td>Sent?</td>\n";
    echo "<td>Blocked?</td>\n";
    echo "<td>Created</td>\n";
    echo "<td>Test</td>\n";
    echo "<td>Last DL</td>";
    echo "<td>DL Count</td></tr>\n";


    for( $i=0; $i<$numRows; $i++ ) {
        $ticket_id = mysql_result( $result, $i, "ticket_id" );
        $sale_date = mysql_result( $result, $i, "sale_date" );
        $lastDL = mysql_result( $result, $i, "last_download_date" );
        $count = mysql_result( $result, $i, "download_count" );
        $name = mysql_result( $result, $i, "name" );
        $email = mysql_result( $result, $i, "email" );
        $tag = mysql_result( $result, $i, "tag" );
        $blocked = mysql_result( $result, $i, "blocked" );
        $sent = mysql_result( $result, $i, "email_sent" );

        $block_toggle = "";
        
        if( $blocked ) {
            $blocked = "BLOCKED";
            $block_toggle = "<a href=\"server.php?action=block_ticket_id&".
                "blocked=0&ticket_id=$ticket_id&password=$password\">unblock</a>";
            
            }
        else {
            $blocked = "";
            $block_toggle = "<a href=\"server.php?action=block_ticket_id&".
                "blocked=1&ticket_id=$ticket_id&password=$password\">block</a>";
            
            }

        $sent_toggle = "";

        if( $sent ) {
            $sent_toggle = "X";
            }
        

        
        echo "<tr>\n";
        
        echo "<td><b>$ticket_id</b> ($tag) ";
        echo "[<a href=\"server.php?action=show_detail&password=$password" .
            "&ticket_id=$ticket_id\">detail</a>]</td>\n";
        echo "<td>$name</td>\n";
        echo "<td>$email</td>\n";
        echo "<td align=center>$sent_toggle</td>\n";
        echo "<td align=right>$blocked [$block_toggle]</td>\n";
        echo "<td>$sale_date</td> ";
        echo "<td>[<a href=\"server.php?action=show_downloads".
            "&ticket_id=$ticket_id\">run test</a>]</td>";
        echo "<td>$lastDL</td>";
        echo "<td>$count DLs</td>";
        
        echo "</tr>\n";
        }
    echo "</table>";


    echo "<hr>";

        // put forms in a table
    echo "<center><table border=1 cellpadding=10><tr>\n";
    


    // fake a security hashes to include in form
    global $fastspringPrivateKeys;
    
    $data = "abc";

    
    // form for force-creating a new id
?>
        <td>
        Create new Ticket:<br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="security_data" VALUE="<?php echo $data;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="sell_ticket">
    Email:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="email"><br>
    Name:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="name"><br>
    Order #:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="reference"><br>
    Tag:
    <SELECT NAME="tags">
<?php

    // auto-gen a drop-down list of available tags
    global $allowedDownloadDates;
    
    foreach( $allowedDownloadDates as $tag => $date ){
        echo "<OPTION VALUE=\"$tag\">$tag</OPTION>";
        }
?>
    </SELECT><br>
    Fake security hash:
    <SELECT NAME="security_hash">
<?php

    // auto-gen a drop-down list of hashes for available tags
    global $allowedDownloadDates;

    foreach( $allowedDownloadDates as $tag => $date ){
        $string_to_hash = $data . $fastspringPrivateKeys[$tag];
        
        $hash = md5( $string_to_hash );

        echo "<OPTION VALUE=\"$hash\">$tag</OPTION>";
        }
?>
    </SELECT><br>
          
    <INPUT TYPE="Submit" VALUE="Generate">
    </FORM>
        </td>
<?php




    // form for sending out download emails
?>
        <td>
        Send download emails:<br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="send_group_email">
    Tag:
    <SELECT NAME="tag">
<?php

    // auto-gen a drop-down list of available tags
    global $allowedDownloadDates;
    
    foreach( $allowedDownloadDates as $tag => $date ){
        echo "<OPTION VALUE=\"$tag\">$tag</OPTION>";
        }
?>
    </SELECT><br>
    Batch size:      
    <INPUT TYPE="text" MAXLENGTH=10 SIZE=10 NAME="batch_size" VALUE="10"><br>
    <INPUT TYPE="checkbox" NAME="confirm" VALUE=1> Confirm<br>      
    <INPUT TYPE="Submit" VALUE="Send">
    </FORM>
        </td>
<?php

    echo "</tr></table></center>\n";

    
    
    echo "<hr>";

?>
    <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="send_all_note">
    Subject:
    <INPUT TYPE="text" MAXLENGTH=80 SIZE=40 NAME="message_subject"><br>
    Tag:
    <SELECT NAME="tag">
<?php
    // auto-gen ALL tags for batches

    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."tickets ".
         "WHERE blocked = '0' AND email_opt_in = '1';";
    $result = ts_queryDatabase( $query );
    $totalTickets = mysql_result( $result, 0, 0 );

    $numToSkip = 0;
    global $emailMaxBatchSize;
    
    while( $totalTickets > 0 ) {
        echo "<OPTION VALUE=\"ALL_BATCH_$numToSkip\">".
            "ALL_BATCH_$numToSkip</OPTION>";
        $totalTickets -= $emailMaxBatchSize;
        $numToSkip += $emailMaxBatchSize;
        }
    
         
    // auto-gen a drop-down list of available tags
    global $allowedDownloadDates;
    
    foreach( $allowedDownloadDates as $tag => $date ){
        echo "<OPTION VALUE=\"$tag\">$tag</OPTION>";
        }
?>
    </SELECT><br>
     Message:<br>
         <TEXTAREA NAME="message_text" COLS=50 ROWS=10></TEXTAREA><br>
    <INPUT TYPE="checkbox" NAME="confirm" VALUE=1> Confirm<br>      
    <INPUT TYPE="Submit" VALUE="Send">
    </FORM>
    <hr>
<?php


?>
    <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="send_all_file_note">
    Subject:
    <INPUT TYPE="text" MAXLENGTH=80 SIZE=40 NAME="message_subject"><br>
    File Downloaded:
    <SELECT NAME="file_name">
<?php

    // auto-gen a drop-down list of files that have ever been downloaded
    
	$query = "SELECT DISTINCT file_name FROM $tableNamePrefix"."downloads;";
    $result = ts_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

	for( $i=0; $i<$numRows; $i++ ) {
        $file_name = mysql_result( $result, $i, "file_name" );
        echo "<OPTION VALUE=\"$file_name\">$file_name</OPTION>";
        }
?>
    </SELECT><br>
     Message:<br>
         <TEXTAREA NAME="message_text" COLS=50 ROWS=10></TEXTAREA><br>
    <INPUT TYPE="checkbox" NAME="confirm" VALUE=1> Confirm<br>      
    <INPUT TYPE="Submit" VALUE="Send">
    </FORM>
    <hr>
<?php



    
    echo "<a href=\"server.php?action=show_log&password=$password\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    }



function ts_showDetail() {
    $password = ts_checkPassword( "show_detail" );

     echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;
    

    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }

    $ticket_id = strtoupper( $ticket_id );


    // form for sending out download emails
?>
        <hr>
        Send download email:<br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="send_single_email">
    <INPUT TYPE="hidden" NAME="ticket_id" VALUE="<?php echo $ticket_id;?>">
    <INPUT TYPE="checkbox" NAME="confirm" VALUE=1> Confirm<br>      
    <INPUT TYPE="Submit" VALUE="Send">
    </FORM>
        <hr>
<?php

            
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
            "WHERE ticket_id = '$ticket_id';";
    $result = ts_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    $row = mysql_fetch_array( $result, MYSQL_ASSOC );

    $email = $row[ "email" ];
    $name = $row[ "name" ];
    $order_number = $row[ "order_number" ];
    $tag = $row[ "tag" ];
            
    // form for editing ticket data
?>
        <hr>
        Edit ticket:<br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="password" VALUE="<?php echo $password;?>">
    <INPUT TYPE="hidden" NAME="action" VALUE="edit_ticket">
    <INPUT TYPE="hidden" NAME="ticket_id" VALUE="<?php echo $ticket_id;?>">
    Email:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="email"
            VALUE="<?php echo $email;?>"><br>
    Name:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="name"
            VALUE="<?php echo $name;?>"><br>
    Order #:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=40 NAME="reference"
            VALUE="<?php echo $order_number;?>"><br>
    Tag:
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="tag"
            VALUE="<?php echo $tag;?>"><br>          
    <INPUT TYPE="Submit" VALUE="Update">
    </FORM>
        <hr>
<?php
            
    
    
    
    $query = "SELECT * FROM $tableNamePrefix"."downloads ".
        "WHERE ticket_id = '$ticket_id' ORDER BY download_date DESC;";
    $result = ts_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    echo "$numRows downloads for $ticket_id:";

    echo " [<a href=\"server.php?action=delete_ticket_id" .
        "&password=$password" .
        "&ticket_id=$ticket_id\">DELETE this id</a>]";
    
    echo "<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $date = mysql_result( $result, $i, "download_date" );
        $ipAddress = mysql_result( $result, $i, "ip_address" );
        $file_name = mysql_result( $result, $i, "file_name" );

        $blocked = mysql_result( $result, $i, "blocked" );

        if( $blocked ) {
            $blocked = "BLOCKED";
            }
        else {
            $blocked = "";
            }
        
        echo "<b>$date</b>: $ipAddress ($file_name) $blocked<hr>\n";
        }
    }



function ts_sendGroupEmail() {
    $password = ts_checkPassword( "send_group_email" );

    
    echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $confirm = "";
    if( isset( $_REQUEST[ "confirm" ] ) ) {
        $confirm = $_REQUEST[ "confirm" ];
        }

    if( $confirm != 1 ) {
        echo "You must check the Confirm box to send emails\n";
        return;
        }
    

    $batch_size = "";
    if( isset( $_REQUEST[ "batch_size" ] ) ) {
        $batch_size = $_REQUEST[ "batch_size" ];
        }


    $tag = "";
    if( isset( $_REQUEST[ "tag" ] ) ) {
        $tag = $_REQUEST[ "tag" ];
        }

    $batchClause = "";
    if( $batch_size > 0 ) {
        $batchClause = " LIMIT 0, $batch_size ";
        }
    
    

    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE tag = '$tag' AND email_sent = '0' AND blocked = '0' ".
        "ORDER BY sale_date ASC $batchClause;";

    ts_sendEmail_q( $query );
    }



function ts_sendSingleEmail() {
    $password = ts_checkPassword( "send_group_email" );

    
    echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $confirm = "";
    if( isset( $_REQUEST[ "confirm" ] ) ) {
        $confirm = $_REQUEST[ "confirm" ];
        }

    if( $confirm != 1 ) {
        echo "You must check the Confirm box to send emails\n";
        return;
        }

    
    $ticket_id = "";
    if( isset( $_REQUEST[ "ticket_id" ] ) ) {
        $ticket_id = $_REQUEST[ "ticket_id" ];
        }


    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";
    ts_sendEmail_q( $query );
    }



// sends download emails for every result in a SQL query
function ts_sendEmail_q( $inQuery ) {
    global $tableNamePrefix;
    
    $result = ts_queryDatabase( $inQuery );
    
    $numRows = mysql_numrows( $result );

    echo "Based on query, sending $numRows emails:<br><br><br>\n";

    for( $i=0; $i<$numRows; $i++ ) {
        $ticket_id = mysql_result( $result, $i, "ticket_id" );
        $name = mysql_result( $result, $i, "name" );
        $email = mysql_result( $result, $i, "email" );

        echo "[$i] Sending email to $email for ticket $ticket_id ... ";
        
        $emailResult = ts_sendEmail_p( $ticket_id, $name, $email );

        if( $emailResult ) {
            echo "SUCCESS";

            $queryB = "UPDATE $tableNamePrefix"."tickets SET " .
                "email_sent = '1' " .
                "WHERE ticket_id = '$ticket_id';";
        
            $resultB = ts_queryDatabase( $queryB );
            }
        else {
            echo "FAILURE";
            }
        echo "<br><br>\n";
        flush();
        }
    }



// sends a download email for a ticket
function ts_sendEmail( $inTickeID ) {

    global $tableNamePrefix;
    
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE ticket_id = '$ticket_id';";
    $result = ts_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    if( $numRows == 1 ) {
        
        $row = mysql_fetch_array( $result, MYSQL_ASSOC );

        $email = $row[ "email" ];
        $name = $row[ "name" ];

        return ts_sendEmail_p( $inTickeID, $name, $email );
        }
    return 0;
    }



// sends a download email for a ticket
function ts_sendEmail_p( $inTickeID, $inName, $inEmail ) {
        
    
    global $siteName, $fullServerURL, $mainSiteURL, $siteEmailAddress;
    //$mailHeaders = "From: $siteEmailAddress";
    $mailHeaders = "From: $siteEmailAddress";

    $downloadURL = $fullServerURL.
        "?action=show_downloads&ticket_id=$inTickeID";

    $mailSubject = "Your [$siteName] download is ready";
    
    $mailBody = "$inName:\n\n".
        "Please share the following information with a friend, so that you ".
        "have someone to play the game with.\n\n".
        "Your can now access your download at:\n\n".
        "  $downloadURL\n\n".
        "You can also access your download manually by ".
        "entering your ticket $inTickeID here:\n\n".
        "  $mainSiteURL\n\n";
    

    /*
    echo "\n<br>Sending mail to: $inEmail<br>\n";
    echo "Subject: $mailSubject<br>\n";
    echo "Headers: $mailHeaders<br>\n";
    echo "Body: <br>\n<pre>$mailBody</pre><br>\n";
    */
    
    $result = mail( $inEmail,
                    $mailSubject,
                    $mailBody,
                    $mailHeaders );
    return $result;
    }



function ts_sendAllNote() {
    $password = ts_checkPassword( "send_all_note" );

    
    echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $confirm = "";
    if( isset( $_REQUEST[ "confirm" ] ) ) {
        $confirm = $_REQUEST[ "confirm" ];
        }

    if( $confirm != 1 ) {
        echo "You must check the Confirm box to send emails\n";
        return;
        }
    

    $message_subject = "";
    if( isset( $_REQUEST[ "message_subject" ] ) ) {
        $message_subject = $_REQUEST[ "message_subject" ];
        }

    $message_subject = stripslashes( $message_subject );
    

    $message_text = "";
    if( isset( $_REQUEST[ "message_text" ] ) ) {
        $message_text = $_REQUEST[ "message_text" ];
        }

    $message_text = stripslashes( $message_text );
    
    

    $tag = "";
    if( isset( $_REQUEST[ "tag" ] ) ) {
        $tag = $_REQUEST[ "tag" ];
        }


    
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE tag = '$tag' AND blocked = '0' AND email_opt_in = '1' ".
        "ORDER BY sale_date ASC;";


    $tagParts = preg_split( "/_/", $tag );

    if( count( $tagParts ) == 3 ) {
        
        if( $tagParts[0] == "ALL" && $tagParts[1] == "BATCH" ) {

            // Only send one batch now, according to tag
            $numToSkip = $tagParts[2];

            global $emailMaxBatchSize;
            
            $query = "SELECT * FROM $tableNamePrefix"."tickets ".
                "WHERE blocked = '0' AND email_opt_in = '1' ".
                "ORDER BY sale_date ASC LIMIT $numToSkip, $emailMaxBatchSize;";
            }
        }
    
    
    
    ts_sendNote_q( $query, $message_subject, $message_text );
    }



function ts_sendAllFileNote() {
    $password = ts_checkPassword( "send_all_note" );

    
    echo "[<a href=\"server.php?action=show_data&password=$password" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $confirm = "";
    if( isset( $_REQUEST[ "confirm" ] ) ) {
        $confirm = $_REQUEST[ "confirm" ];
        }

    if( $confirm != 1 ) {
        echo "You must check the Confirm box to send emails\n";
        return;
        }
    

    $message_subject = "";
    if( isset( $_REQUEST[ "message_subject" ] ) ) {
        $message_subject = $_REQUEST[ "message_subject" ];
        }

    $message_subject = stripslashes( $message_subject );
    

    $message_text = "";
    if( isset( $_REQUEST[ "message_text" ] ) ) {
        $message_text = $_REQUEST[ "message_text" ];
        }

    $message_text = stripslashes( $message_text );
    
    

    $file_name = "";
    if( isset( $_REQUEST[ "file_name" ] ) ) {
        $file_name = $_REQUEST[ "file_name" ];
        }

    	
    $query = "SELECT DISTINCT email, name FROM $tableNamePrefix"."downloads ".
        "LEFT JOIN $tableNamePrefix"."tickets ON ".
        "$tableNamePrefix"."downloads.ticket_id = ".
        "$tableNamePrefix"."tickets.ticket_id ".
        "WHERE file_name='$file_name';";
/*
    $query = "SELECT * FROM $tableNamePrefix"."tickets ".
        "WHERE tag = '$tag' AND blocked = '0' AND email_opt_in = '1';";
*/
    ts_sendNote_q( $query, $message_subject, $message_text );
    }



// sends note emails for every result in a SQL query
function ts_sendNote_q( $inQuery, $message_subject, $message_text ) {
    global $tableNamePrefix;
    
    $result = ts_queryDatabase( $inQuery );
    
    $numRows = mysql_numrows( $result );

    echo "Query is:<br>$inQuery<br><br>";

    echo "Based on query, sending $numRows emails:<br><br><br>\n";

    for( $i=0; $i<$numRows; $i++ ) {
        $name = mysql_result( $result, $i, "name" );
        $email = mysql_result( $result, $i, "email" );

        echo "[$i] Sending note to $email ... ";
        
        $emailResult = ts_sendNote_p( $message_subject, $message_text,
                                      $name, $email );

        if( $emailResult ) {
            echo "SUCCESS";
            }
        else {
            echo "FAILURE";
            }
        echo "<br><br>\n";
        flush();
        }
    }


// sends a note email to a specific name address
function ts_sendNote_p( $message_subject, $message_text, $inName, $inEmail ) {
        
    
    global $siteName, $fullServerURL, $mainSiteURL, $siteEmailAddress;
    //$mailHeaders = "From: $siteEmailAddress";
    $mailHeaders = "From: $siteEmailAddress";


    $mailSubject = $message_subject;
    
    $mailBody = "$inName:\n\n". $message_text ."\n\n";
    

    /*
    echo "\n<br>Sending mail to: $inEmail<br>\n";
    echo "Subject: $mailSubject<br>\n";
    echo "Headers: $mailHeaders<br>\n";
    echo "Body: <br>\n<pre>$mailBody</pre><br>\n";
    */
    
    $result = mail( $inEmail,
                    $mailSubject,
                    $mailBody,
                    $mailHeaders );
    return $result;
    }







// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function ts_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName;
    
    
    mysql_connect( $databaseServer, $databaseUsername, $databasePassword )
        or ts_fatalError( "Could not connect to database server: " .
                       mysql_error() );
    
	mysql_select_db( $databaseName )
        or ts_fatalError( "Could not select $databaseName database: " .
                       mysql_error() );
    }


 
/**
 * Closes the database connection.
 */
function ts_closeDatabase() {
    mysql_close();
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function ts_queryDatabase( $inQueryString ) {

    $result = mysql_query( $inQueryString )
        or ts_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
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
function ts_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = ts_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = mysql_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function ts_log( $message ) {
    global $enableLog, $tableNamePrefix;

    $slashedMessage = addslashes( $message );
    
    if( $enableLog ) {
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = ts_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function ts_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    ts_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function ts_operationError( $message ) {
    
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
function ts_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'ts_addslashes_deep', $inValue )
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
function ts_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'sb_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



function ts_checkPassword( $inFunctionName ) {
    $password = "";
    if( isset( $_REQUEST[ "password" ] ) ) {
        $password = $_REQUEST[ "password" ];
        }

    global $accessPassword, $tableNamePrefix, $remoteIP;
    
    if( $password != $accessPassword ) {
        echo "Incorrect password.";

        ts_log( "Failed $inFunctionName access with password:  $password" );

        die();
        }

    return $password;
    }




// found here:
// http://php.net/manual/en/function.fpassthru.php

function ts_send_file( $path ) {
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
