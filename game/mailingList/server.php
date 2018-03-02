<?php



global $ml_version;
$ml_version = "1";


// enable verbose error reporting to detect uninitialized variables
error_reporting( E_ALL );

// edit settings.php to change server' settings
include( "settings.php" );
include( "$bulkEmailerPath" );




// no end-user settings below this point



// no caching
//header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache'); 







// page layout for web-based setup
$setup_header = "
<HTML>
<HEAD><TITLE>Mailing List Server Web-based setup</TITLE></HEAD>
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
    $_GET     = array_map( 'ml_stripslashes_deep', $_GET );
    $_POST    = array_map( 'ml_stripslashes_deep', $_POST );
    $_REQUEST = array_map( 'ml_stripslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'ml_stripslashes_deep', $_COOKIE );
    }
    





// all calls need to connect to DB, so do it once here
$ml_mysqlLink;

ml_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = ml_requestFilter( "action", "/[A-Z_]+/i" );

$debug = ml_requestFilter( "debug", "/[01]/" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }




if( $action == "version" ) {
    global $ml_version;
    echo "$ml_version";
    }
else if( $action == "timestamp" ) {
    echo time();
    }
else if( $action == "show_log" ) {
    ml_showLog();
    }
else if( $action == "clear_log" ) {
    ml_clearLog();
    }
else if( $action == "subscribe" ) {
    ml_subscribe();
    }
else if( $action == "subscribe_step_b" ) {
    ml_subscribeStepB();
    }
else if( $action == "mass_subscribe" ) {
    ml_massSubscribe();
    }
else if( $action == "confirm" ) {
    ml_confirm();
    }
else if( $action == "remove" ) {
    ml_remove();
    }
else if( $action == "mass_remove" ) {
    ml_massRemove();
    }
else if( $action == "show_data" ) {
    ml_showData();
    }
else if( $action == "send_message" ) {
    ml_sendMessage();
    }
else if( $action == "logout" ) {
    ml_logout();
    }
else if( $action == "ml_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Mailing List Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    ml_setupDatabase();

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
    $tableName = $tableNamePrefix . "recipients";
    
    $exists = ml_doesTableExist( $tableName );
        
    if( $exists  ) {
        echo "Mailing List Server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Mailing List Server Web-based Setup</H2>";
    
        echo "Mailing List Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=ml_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

ml_closeDatabase();







/**
 * Creates the database tables needed by seedBlogs.
 */
function ml_setupDatabase() {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . "log";
    if( ! ml_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = ml_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "recipients";
    if( ! ml_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "confirmation_code CHAR(10) NOT NULL PRIMARY KEY," .
            "email CHAR(255) NOT NULL," .
            "UNIQUE KEY( email )," .
            "confirmed TINYINT NOT NULL," .
            "creation_date DATETIME NOT NULL," .
            "last_sent_date DATETIME NOT NULL," .
            "sent_count INT NOT NULL );";

        $result = ml_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }



function ml_showLog() {
    ml_checkPassword( "show_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";
    $result = ml_queryDatabase( $query );

    $numRows = mysql_numrows( $result );



    echo "<a href=\"server.php?action=clear_log\">".
        "Clear log</a>";
        
    echo "<hr>";
        
    echo "$numRows log entries:<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $time = mysql_result( $result, $i, "entry_time" );
        $entry = mysql_result( $result, $i, "entry" );

        echo "<b>$time</b>:<br>$entry<hr>\n";
        }
    }



function ml_clearLog() {
    ml_checkPassword( "clear_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = ml_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }







function ml_subscribe() {
    global $tableNamePrefix, $remoteIP, $header, $footer, $fullServerURL;


    $timeStamp = ml_requestFilter( "timeStamp", "/[0-9]+/", time() );

    $currentTime = time();

    if( $timeStamp > $currentTime - 5 ) {
        $seconds = $currentTime - $timeStamp;
        
        eval( $header );
            
        echo "You did that way faster than a human would.";
        
        eval( $footer );

        $email = "";
        if( isset( $_REQUEST[ "email" ] ) ) {
            $email = $_REQUEST[ "email" ];
            }
        
        ml_log( "Email '$email' signed up too fast ($seconds sec) ".
                "after loading form." );
        return;
        }
    else if( $timeStamp < $currentTime - 1800 ) {
        $seconds = $currentTime - $timeStamp;
        
        eval( $header );
            
        echo "Your submission is stale.  Please go back and reload the page.";
        
        eval( $footer );

        $email = "";
        if( isset( $_REQUEST[ "email" ] ) ) {
            $email = $_REQUEST[ "email" ];
            }
        
        ml_log( "Email '$email' signed up too slow ($seconds sec) ".
                "after loading form." );
        return;
        }

    $email = "";
    if( isset( $_REQUEST[ "email" ] ) ) {
        $email = $_REQUEST[ "email" ];
        }

    eval( $header );

    echo "Are you human?<br><br>";

    ?>
    <form action="<?php echo $fullServerURL;?>" method="post">
         <input type="hidden" name="action" value="subscribe_step_b">
         <input type="hidden" name="email" value="<?php echo $email;?>">
         What has leaves, rhymes with "free," and starts with a "t"?<br><br>
         Answer: <input type="text" name="human_test" value="" size=6>
         <input type="submit" value="Submit">
         </form>
    <?php
         
    eval( $footer );
    
    }

    
function ml_subscribeStepB() {
    
    // input filtering handled below
    $email = "";
    if( isset( $_REQUEST[ "email" ] ) ) {
        $email = $_REQUEST[ "email" ];
        }

    $human_test = "";
    if( isset( $_REQUEST[ "human_test" ] ) ) {
        $human_test = $_REQUEST[ "human_test" ];
        $human_test = strtolower( $human_test );
        }

    if( $human_test != "tree" ) {
        echo "You failed the human test.";

        ml_log( "Human test failed for ".
                "email '$email' with answer '$human_test'" );
        return;
        }
    
    
    ml_createSubscription( $email, 0, 0 );
    }




function ml_massSubscribe() {
    global $tableNamePrefix, $remoteIP, $header, $footer;

    ml_checkPassword( "mass subscribe" );

    $confirmed = ml_requestFilter( "confirmed", "/1/", "0" );


    // input filtering handled below
    $emails = "";
    if( isset( $_REQUEST[ "emails" ] ) ) {
        $emails = $_REQUEST[ "emails" ];
        }
    
    $emailArray = preg_split( "/\s+/", $emails );

    $failedCount = 0;
    $alreadySubscribedCount = 0;
    $successCount = 0;
    foreach( $emailArray as $email ) {
        $result = ml_createSubscription( $email, $confirmed, 1 );

        switch( $result ) {
            case 0:
                $failedCount++;
                break;
            case 1:
                $successCount++;
                break;
            case 2:
                $alreadySubscribedCount++;
                break;
            }   
        }

    $subscriptionWord = "Subscriptions";
    if( $successCount == 1 ) {
        $subscriptionWord = "Subscription";
        }
    
    echo "Summary:  $failedCount Failed, ".
        "$alreadySubscribedCount Already Subscribed, ".
        "$successCount $subscriptionWord Created";
    echo "<hr>";
    ml_showData( false );
    }




// utility function used by both massSubscribe and subscribe
// handles filtering of email address
// returns 0 on invalid
// returns 2 on already subscribed
// returns 1 on success
function ml_createSubscription( $email, $confirmed, $manual ) {
    global $tableNamePrefix, $remoteIP, $header, $footer;

    $unfilteredEmail = $email;
    $email = ml_filter( $email, "/[A-Z0-9._%+-]+@[A-Z0-9.-]+/i", "" );

    if( $email == "" ) {
        if( $manual ) {
            echo "Invalid email address: $unfilteredEmail<br>";
            }
        else {
            eval( $header );
            
            echo "Invalid email address: <b>$unfilteredEmail</b>";
            
            eval( $footer );
            }
        return 0;
        }   

    
    $email = strtolower( $email );
    
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."recipients ".
        "WHERE email='$email';";
    
    $result = ml_queryDatabase( $query );
    $hitCount = mysql_result( $result, 0, 0 );


    if( $hitCount > 0 ) {

        if( $manual ) {
            echo "Already subscribed: $email<br>";
            }
        else {
            eval( $header );
            
            echo "You're already subscribed: <b>$email</b>";
            
            eval( $footer );
            }
        return 2;
        }
    
    

    $found_unused_code = 0;
    $salt = 0;

    global $confirmationCodeSalt;
    
    while( ! $found_unused_code ) {

        $randVal = rand();
        
        $hash = md5( $email .
                     uniqid( "$randVal"."$salt"."$confirmationCodeSalt",
                             true ) );

        $hash = strtoupper( $hash );
        
        
        $confirmation_code = substr( $hash, 0, 10 );


        
        // make code more human-friendly (alpha only)

        $digitArray =
            array( "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" );
        $letterArray =
            array( "W", "H", "J", "K", "X", "M", "N", "P", "T", "Y" );

        $confirmation_code = str_replace( $digitArray, $letterArray,
                                          $confirmation_code );

        $query = "INSERT INTO $tableNamePrefix". "recipients VALUES ( " .
            "'$confirmation_code', '$email', $confirmed, ".
            "CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 0 );";


        $result = ml_queryDatabase( $query );

        if( $result ) {
            $found_unused_code = 1;

            ml_log( "subscription for $email [$confirmation_code] ".
                    "created by $remoteIP" );

            global $subscriptionMessage, $header, $footer,
                $listName, $fullServerURL;

            if( $manual ) {
                echo "Subscribed $email<br>";
                }
            else {
                eval( $header );
            
                echo "$subscriptionMessage";
                
                eval( $footer );
                }

            if( ! $confirmed ) {
                global $firstMessageExtraA, $firstMessageExtraB;
                
                be_addMessage( "Subscription:  $listName",
                               $firstMessageExtraA.
                               "To confirm your subscription, ".
                               "follow this link:\n\n".
                               "$fullServerURL?action=confirm".
                               "&code=$confirmation_code\n\n".
                               $firstMessageExtraB,
                               array( $email ),
                               array( "" ),
                               array( "" ) );
                }

            return 1;
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





function ml_confirm() {
    global $tableNamePrefix, $remoteIP, $header, $footer;
    $code = ml_requestFilter( "code", "/[A-Z]+/" );

    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."recipients ".
        "WHERE confirmation_code='$code' AND confirmed = 1;";
    
    $result = ml_queryDatabase( $query );
    $hitCount = mysql_result( $result, 0, 0 );

    $alreadyConfirmed = false;
    
    if( $hitCount > 0 ) {
        $alreadyConfirmed = true;
        }
    
    
    $query = "UPDATE $tableNamePrefix"."recipients ".
        "SET confirmed = 1 WHERE confirmation_code = '$code';";
    
    $result = ml_queryDatabase( $query );
    $hitCount = mysql_affected_rows();


    eval( $header );
    
    if( $hitCount == 0 ) {
        if( $alreadyConfirmed ) {
            echo "You already confirmed your subscription.";
            }
        else {
            echo "Confirmation failed.";
            }
        }
    else {
        global $confirmationMessage;
        echo "$confirmationMessage";
        }
    
    eval( $footer );
    }




function ml_remove() {
    global $tableNamePrefix, $remoteIP, $header, $footer;
    $code = ml_requestFilter( "code", "/[A-Z]+/" );
    $manual = ml_requestFilter( "manual", "/1/" );
    $confirm = ml_requestFilter( "confirm", "/1/" );

    if( $manual == 1 ) {
        ml_checkPassword( "manual remove" );

        if( $confirm != 1 ) {

            echo "Confirmation checkbox required<hr>";
        
            ml_showData( false );
            return;
            }
        }

    $query = "SELECT email FROM $tableNamePrefix"."recipients ".
        "WHERE confirmation_code = '$code';";
    
    $result = ml_queryDatabase( $query );
    $email = "";
    if( mysql_affected_rows() > 0 ) {
        $email = mysql_result( $result, 0, "email" );
        }
    
    
    $query = "DELETE FROM $tableNamePrefix"."recipients ".
        "WHERE confirmation_code = '$code';";
    
    $result = ml_queryDatabase( $query );
    $hitCount = mysql_affected_rows();

    if( $hitCount == 1 ) {
        ml_log( "subscription for $email [$code] ".
                "removed by $remoteIP" );
        }
    

    if( $manual == 1 ) {

        if( $hitCount == 0 ) {
            echo "Removal failed<hr>";
            }
        else {
            echo "Removal succeeded<hr>";
            }
        ml_showData( false );
        return;
        }
    
    eval( $header );
    
    if( $hitCount == 0 ) {
        echo "Your subscription was not found.";
        }
    else {
        global $removalMessage;
        echo "$removalMessage";
        }
    
    eval( $footer );
    }




function ml_massRemove() {
    global $tableNamePrefix, $remoteIP, $header, $footer;

    ml_checkPassword( "mass remove" );

    $confirm = ml_requestFilter( "confirm", "/1/" );


     if( $confirm != 1 ) {
         echo "Confirmation checkbox required<hr>";
         
         ml_showData( false );
         return;
         }

    // input filtering handled below
    $emails = "";
    if( isset( $_REQUEST[ "emails" ] ) ) {
        $emails = $_REQUEST[ "emails" ];
        }
    
    $emailArray = preg_split( "/\s+/", $emails );

    $failedCount = 0;
    $successCount = 0;
    foreach( $emailArray as $email ) {

        $unfilteredEmail = $email;
        $email = ml_filter( $email, "/[A-Z0-9._%+-]+@[A-Z0-9.-]+/i", "" );

        if( $email == "" ) {
            echo "Invalid email address: $unfilteredEmail<br>";
            $failedCount++;
            }
        else {
            $query = "DELETE FROM $tableNamePrefix"."recipients ".
                "WHERE email = '$email';";
    
            $result = ml_queryDatabase( $query );
            $hitCount = mysql_affected_rows();

            if( $hitCount == 1 ) {
                ml_log( "subscription for $email ".
                        "mass removed by $remoteIP" );
                echo "Removed: $email<br>";
                $successCount++;
                }
            else {
                echo "Not found: $email<br>";
                $failedCount++;
                }
            }
        }

    $subscriptionWord = "Subscriptions";
    if( $successCount == 1 ) {
        $subscriptionWord = "Subscription";
        }
    
    echo "Summary:  $failedCount Failed, ".
        "$successCount $subscriptionWord Removed";
    echo "<hr>";
    ml_showData( false );
    }










function ml_logout() {

    ml_clearPasswordCookie();

    echo "Logged out";
    }




function ml_showData( $checkPassword = true ) {
    // these are global so they work in embeded function call below
    global $skip, $search, $order_by;

    if( $checkPassword ) {
        ml_checkPassword( "show_data" );
        }
    
    global $tableNamePrefix, $remoteIP;
    

    echo "<table width='100%' border=0><tr>".
        "<td>[<a href=\"server.php?action=show_data" .
            "\">Main</a>]</td>".
        "<td align=right>[<a href=\"server.php?action=logout" .
            "\">Logout</a>]</td>".
        "</tr></table><br><br><br>";




    $skip = ml_requestFilter( "skip", "/[0-9]+/", 0 );
    
    global $recordsPerPage;
    
    $search = ml_requestFilter( "search", "/[A-Z0-9_@. -]+/i" );

    $order_by = ml_requestFilter( "order_by", "/[A-Z_]+/i",
                                  "creation_date" );
    
    $keywordClause = "";
    $searchDisplay = "";
    
    if( $search != "" ) {
        

        $keywordClause = "WHERE ( email LIKE '%$search%' " .
            "OR confirmation_code LIKE '%$search%' ".
            "OR sent_count LIKE '%$search%' ) ";

        $searchDisplay = " matching <b>$search</b>";
        }
    

    

    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."recipients ".
        "$keywordClause;";

    $result = ml_queryDatabase( $query );
    $totalRecipients = mysql_result( $result, 0, 0 );


    $orderDir = "DESC";

    if( $order_by == "confirmation_code" || $order_by == "email" ) {
        $orderDir = "ASC";
        }
    
             
    $query = "SELECT * FROM $tableNamePrefix"."recipients $keywordClause".
        "ORDER BY $order_by $orderDir ".
        "LIMIT $skip, $recordsPerPage;";
    $result = ml_queryDatabase( $query );
    
    $numRows = mysql_numrows( $result );

    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $recordsPerPage - 1;

    if( $endSkip > $totalRecipients ) {
        $endSkip = $totalRecipients;
        }



        // form for searching recipients
?>
        <hr>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="show_data">
    <INPUT TYPE="hidden" NAME="order_by" VALUE="<?php echo $order_by;?>">
    <INPUT TYPE="text" MAXLENGTH=40 SIZE=20 NAME="search"
             VALUE="<?php echo $search;?>">
    <INPUT TYPE="Submit" VALUE="Search">
    </FORM>
        <hr>
<?php

    

    
    echo "$totalRecipients active recipients". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $recordsPerPage;

    $prevSkip = $skip - $recordsPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$prevSkip&search=$search&order_by=$order_by\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalRecipients ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$nextSkip&search=$search&order_by=$order_by\">".
            "Next Page</a>]";
        }

    echo "<br><br>";
    
    echo "<table border=1 cellpadding=5>\n";

    function orderLink( $inOrderBy, $inLinkText ) {
        global $skip, $search, $order_by;
        if( $inOrderBy == $order_by ) {
            // already displaying this order, don't show link
            return "<b>$inLinkText</b>";
            }

        // else show a link to switch to this order
        return "<a href=\"server.php?action=show_data" .
            "&search=$search&skip=$skip&order_by=$inOrderBy\">$inLinkText</a>";
        }

    
    echo "<tr>\n";    
    echo "<td>".orderLink( "email", "Email" )."</td>\n";
    echo "<td>".orderLink( "confirmation_code", "Code" )."</td>\n";
    echo "<td>Confirmed?</td>\n";
    echo "<td></td>\n";
    echo "<td>".orderLink( "creation_date", "Created" )."</td>\n";
    echo "<td>".orderLink( "last_sent_date", "Last Sent" )."</td>\n";
    echo "<td>".orderLink( "sent_count", "Message Count" )."</td>\n";
    echo "</tr>\n";


    for( $i=0; $i<$numRows; $i++ ) {
        $email = mysql_result( $result, $i, "email" );
        $confirmation_code = mysql_result( $result, $i, "confirmation_code" );
        $confirmed = mysql_result( $result, $i, "confirmed" );
        $creation_date = mysql_result( $result, $i, "creation_date" );
        $last_sent_date = mysql_result( $result, $i, "last_sent_date" );
        $sent_count = mysql_result( $result, $i, "sent_count" );

        
        $confirmed_toggle = "";

        if( $confirmed ) {
            $confirmed_toggle = "X";
            }
        

        
        echo "<tr>\n";
        
        echo "<td><b>$email</b></td>\n";
        echo "<td>$confirmation_code</td>\n";
        echo "<td>$confirmed_toggle</td>\n";

        echo "<td>";
        echo "<form action='server.php' method='get'>".
            "<input type='hidden' name='action' value='remove'>".
            "<input type='hidden' name='code' value='$confirmation_code'>".
            "<input type='hidden' name='manual' value='1'>".
            "<input type='submit' value='Remove'>".
            "<input type='checkbox' name='confirm' value='1'>".
            "</form>";
        echo "</td>";
        
        echo "<td>$creation_date</td>";
        echo "<td>$last_sent_date</td>";
        echo "<td>$sent_count</td>";
        
        echo "</tr>\n";
        }
    echo "</table>";


    echo "<hr>";

        // put forms in a table
    echo "<center><table border=1 cellpadding=10><tr>\n";
    



    

    // form for force-creating group of new ids
?>
        <td>
        Mass Subscription:<br><br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="mass_subscribe">
             Emails (one per line):<br>
             <TEXTAREA NAME="emails" COLS=30 ROWS=10></TEXTAREA><br>
    <input type='checkbox' name='confirmed' value='1'> Skip email confirmation<br><br>      
    <INPUT TYPE="Submit" VALUE="Create">
    </FORM>
        </td>
<?php
             
    // form for force-removing group of new ids
?>
        <td>
        Mass Removal:<br><br>
            <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="mass_remove">
             Emails (one per line):<br>
             <TEXTAREA NAME="emails" COLS=30 ROWS=10></TEXTAREA><br>
    <input type='checkbox' name='confirm' value='1'> Confirm<br><br>      
    <INPUT TYPE="Submit" VALUE="Remove">
    </FORM>
        </td>
<?php

    echo "</tr></table></center>\n";

    
    
    echo "<hr>";

?>
    <FORM ACTION="server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="send_message">
    Subject:
    <INPUT TYPE="text" MAXLENGTH=80 SIZE=40 NAME="message_subject"><br>

         Message:<br>
         <TEXTAREA NAME="message_text" COLS=80 ROWS=10></TEXTAREA><br>
    <INPUT TYPE="checkbox" NAME="confirm" VALUE=1> Confirm<br>      
    <INPUT TYPE="Submit" VALUE="Send">
    </FORM>
    <hr>
<?php
    echo "<a href=\"server.php?action=show_log\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    }



function ml_sendMessage() {
    ml_checkPassword( "send_message" );

    
    echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $confirm = ml_requestFilter( "confirm", "/[01]/" );
    
    if( $confirm != 1 ) {
        echo "You must check the Confirm box to send emails\n";
        return;
        }


    // pass subject and body through without regex filter
    // these are filtered by bulkEmailerAPI
    // also, they are from a trusted user (password-protected)
    $message_subject = "";
    if( isset( $_REQUEST[ "message_subject" ] ) ) {
        $message_subject = $_REQUEST[ "message_subject" ];
        }
    

    $message_text = "";
    if( isset( $_REQUEST[ "message_text" ] ) ) {
        $message_text = $_REQUEST[ "message_text" ];
        }

    $query = "SELECT email, confirmation_code ".
        "FROM $tableNamePrefix"."recipients ".
        "WHERE confirmed = 1;";

    $result = ml_queryDatabase( $query );

    $numRows = mysql_numrows( $result );

    echo "Adding $numRows emails to the bulkEmailer queue...<br><br><br>\n";

    // use %CUSTOM% replacement feature of bulkEmailer
    // to generate custom unsubscribe links
    
    $allEmails = array();
    $allCodes = array();
    // not used, full of empty strings
    $allCustom2 = array();
    
    for( $i=0; $i<$numRows; $i++ ) {
        $email = mysql_result( $result, $i, "email" );
        $code = mysql_result( $result, $i, "confirmation_code" );
        
        $allEmails[] = $email;
        $allCodes[] = $code;
        $allCustom2[] = "";
        }

    global $fullServerURL;
    
    $message_text =
        $message_text .
        "\n\n--\n" .
        "To unsubscribe, follow this link:\n".
        "$fullServerURL?action=remove&code=%CUSTOM%\n\n";
    
    
    be_addMessage( $message_subject, $message_text, $allEmails, $allCodes,
                   $allCustom2 );

    $query = "UPDATE $tableNamePrefix"."recipients SET ".
        "last_sent_date = CURRENT_TIMESTAMP, ".
        "sent_count = sent_count + 1 ".
        "WHERE confirmed = 1;";
    ml_queryDatabase( $query );
        
    echo "...done<br>";
    }









// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function ml_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName,
        $ml_mysqlLink;
    
    
    $ml_mysqlLink =
        mysql_connect( $databaseServer, $databaseUsername, $databasePassword )
        or ml_operationError( "Could not connect to database server: " .
                              mysql_error() );
    
	mysql_select_db( $databaseName )
        or ml_operationError( "Could not select $databaseName database: " .
                              mysql_error() );
    }


 
/**
 * Closes the database connection.
 */
function ml_closeDatabase() {
    global $ml_mysqlLink;
    
    mysql_close( $ml_mysqlLink );
    }



/**
 * Queries the database, and dies with an error message on failure.
 *
 * @param $inQueryString the SQL query string.
 *
 * @return a result handle that can be passed to other mysql functions.
 */
function ml_queryDatabase( $inQueryString ) {
    global $ml_mysqlLink;

    if( gettype( $ml_mysqlLink ) != "resource" ) {
        // not a valid mysql link?
        ml_connectToDatabase();
        }
    
    $result = mysql_query( $inQueryString, $ml_mysqlLink );
    
    if( $result == FALSE ) {

        $errorNumber = mysql_errno();
        
        // server lost or gone?
        if( $errorNumber == 2006 ||
            $errorNumber == 2013 ||
            // access denied?
            $errorNumber == 1044 ||
            $errorNumber == 1045 ||
            // no db selected?
            $errorNumber == 1046 ) {

            // connect again?
            ml_closeDatabase();
            ml_connectToDatabase();

            $result = mysql_query( $inQueryString, $ml_mysqlLink )
                or ml_operationError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysql_error() );
            }
        else {
            // some other error (we're still connected, so we can
            // add log messages to database
            ml_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                           mysql_error() );
            }
        }    

    return $result;
    }



/**
 * Checks whether a table exists in the currently-connected database.
 *
 * @param $inTableName the name of the table to look for.
 *
 * @return 1 if the table exists, or 0 if not.
 */
function ml_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = ml_queryDatabase( $query );

    $numRows = mysql_numrows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = mysql_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function ml_log( $message ) {
    global $enableLog, $tableNamePrefix, $ml_mysqlLink;

    if( $enableLog ) {
        $slashedMessage = mysql_real_escape_string( $message, $ml_mysqlLink );
    
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = ml_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function ml_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    ml_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function ml_operationError( $message ) {
    
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
function ml_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'ml_addslashes_deep', $inValue )
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
function ml_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'ml_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function ml_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
    if( ! isset( $_REQUEST[ $inRequestVariable ] ) ) {
        return $inDefault;
        }

    return ml_filter( $_REQUEST[ $inRequestVariable ], $inRegex, $inDefault );
    }


/**
 * Filters a value  using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function ml_filter( $inValue, $inRegex, $inDefault = "" ) {
    
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
// (cached by a browser) can't be used to figure out the cookie and log in
// later. 
function ml_checkPassword( $inFunctionName ) {
    $password = "";
    $password_hash = "";

    $badCookie = false;
    
    
    global $accessPassword, $tableNamePrefix, $remoteIP;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    
    if( isset( $_REQUEST[ "password" ] ) ) {
        $password = $_REQUEST[ "password" ];

        // generate a new hash cookie from this password
        $newSalt = time();
        $newHash = md5( $newSalt . $password );
        
        $password_hash = $newSalt . "_" . $newHash;
        }
    else if( isset( $_COOKIE[ $cookieName ] ) ) {
        $password_hash = $_COOKIE[ $cookieName ];
        
        // check that it's a good hash
        
        $hashParts = preg_split( "/_/", $password_hash );

        // default, to show in log message on failure
        // gets replaced if cookie contains a good hash
        $password = "(bad cookie:  $password_hash)";

        $badCookie = true;
        
        if( count( $hashParts ) == 2 ) {
            
            $salt = $hashParts[0];
            $hash = $hashParts[1];
            
            $trueHash = md5( $salt . $accessPassword );
            
            if( $trueHash == $hash ) {
                $password = $accessPassword;
                $badCookie = false;
                }
            }
        }
    else {
        // no request variable, no cookie
        // cookie probably expired
        $badCookie = true;
        $password_hash = "(no cookie.  expired?)";
        }
    
        
    
    if( $password != $accessPassword ) {

        if( ! $badCookie ) {
            
            echo "Incorrect password.";

            ml_log( "Failed $inFunctionName access with password:  ".
                    "$password" );
            }
        else {
            echo "Session expired.";
                
            ml_log( "Failed $inFunctionName access with bad cookie:  ".
                    "$password_hash" );
            }
        
        die();
        }
    else {
        // set cookie again, renewing it, expires in 24 hours
        $expireTime = time() + 60 * 60 * 24;
    
        setcookie( $cookieName, $password_hash, $expireTime, "/" );
        }
    }
 



function ml_clearPasswordCookie() {
    global $tableNamePrefix;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    // expire 24 hours ago (to avoid timezone issues)
    $expireTime = time() - 60 * 60 * 24;

    setcookie( $cookieName, "", $expireTime, "/" );
    }
 
 



// found here:
// http://php.net/manual/en/function.fpassthru.php

function ml_send_file( $path ) {
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
