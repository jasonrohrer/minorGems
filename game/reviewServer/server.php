<?php



global $rs_version;
$rs_version = "1";



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
<HEAD><TITLE>Review Server Web-based setup</TITLE></HEAD>
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
    $_GET     = array_map( 'rs_stripslashes_deep', $_GET );
    $_POST    = array_map( 'rs_stripslashes_deep', $_POST );
    $_REQUEST = array_map( 'rs_stripslashes_deep', $_REQUEST );
    $_COOKIE  = array_map( 'rs_stripslashes_deep', $_COOKIE );
    }
    


// Check that the referrer header is this page, or kill the connection.
// Used to block XSRF attacks on state-changing functions.
// (To prevent it from being dangerous to surf other sites while you are
// logged in as admin.)
// Thanks Chris Cowan.
function rs_checkReferrer() {
    global $fullServerURL;
    
    if( !isset($_SERVER['HTTP_REFERER']) ||
        strpos($_SERVER['HTTP_REFERER'], $fullServerURL) !== 0 ) {
        
        die( "Bad referrer header" );
        }
    }




// all calls need to connect to DB, so do it once here
rs_connectToDatabase();

// close connection down below (before function declarations)


// testing:
//sleep( 5 );


// general processing whenver server.php is accessed directly




// grab POST/GET variables
$action = rs_requestFilter( "action", "/[A-Z_]+/i" );

$debug = rs_requestFilter( "debug", "/[01]/" );

$remoteIP = "";
if( isset( $_SERVER[ "REMOTE_ADDR" ] ) ) {
    $remoteIP = $_SERVER[ "REMOTE_ADDR" ];
    }




if( $action == "version" ) {
    global $rs_version;
    echo "$rs_version";
    }
else if( $action == "get_sequence_number" ) {
    rs_getSequenceNumber();
    }
else if( $action == "log_game" ) {
    rs_logGame();
    }
else if( $action == "submit_review" ) {
    rs_submitReview();
    }
else if( $action == "remove_review" ) {
    rs_removeReview();
    }
else if( $action == "update_html" ) {
    rs_updateHTML();
    }
else if( $action == "show_log" ) {
    rs_showLog();
    }
else if( $action == "clear_log" ) {
    rs_clearLog();
    }
else if( $action == "show_data" ) {
    rs_showData();
    }
else if( $action == "regen_static_html" ) {
    rs_regenStaticHTML();
    }
else if( $action == "show_detail" ) {
    rs_showDetail();
    }
else if( $action == "view_review" ) {
    rs_viewReview();
    }
else if( $action == "list_recent" ) {
    rs_listRecent();
    }
else if( $action == "list_playtime" ) {
    rs_listPlaytime();
    }
else if( $action == "logout" ) {
    rs_logout();
    }
else if( $action == "rs_setup" ) {
    global $setup_header, $setup_footer;
    echo $setup_header; 

    echo "<H2>Review Server Web-based Setup</H2>";

    echo "Creating tables:<BR>";

    echo "<CENTER><TABLE BORDER=0 CELLSPACING=0 CELLPADDING=1>
          <TR><TD BGCOLOR=#000000>
          <TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5>
          <TR><TD BGCOLOR=#FFFFFF>";

    rs_setupDatabase();

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
    $exists = rs_doesTableExist( $tableNamePrefix . "user_stats" ) &&
        rs_doesTableExist( $tableNamePrefix . "log" );
    
        
    if( $exists  ) {
        echo "Review Server database setup and ready";
        }
    else {
        // start the setup procedure

        global $setup_header, $setup_footer;
        echo $setup_header; 

        echo "<H2>Review Server Web-based Setup</H2>";
    
        echo "Review Server will walk you through a " .
            "brief setup process.<BR><BR>";
        
        echo "Step 1: ".
            "<A HREF=\"server.php?action=rs_setup\">".
            "create the database tables</A>";

        echo $setup_footer;
        }
    }



// done processing
// only function declarations below

rs_closeDatabase();







/**
 * Creates the database tables needed by seedBlogs.
 */
function rs_setupDatabase() {
    global $tableNamePrefix;

    $tableName = $tableNamePrefix . "log";
    if( ! rs_doesTableExist( $tableName ) ) {

        // this table contains general info about the server
        // use INNODB engine so table can be locked
        $query =
            "CREATE TABLE $tableName(" .
            "entry TEXT NOT NULL, ".
            "entry_time DATETIME NOT NULL );";

        $result = rs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }

    
    
    $tableName = $tableNamePrefix . "user_stats";
    if( ! rs_doesTableExist( $tableName ) ) {

        // this table contains general info about each ticket
        $query =
            "CREATE TABLE $tableName(" .
            "id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT," .
            "email VARCHAR(254) NOT NULL," .
            "UNIQUE KEY( email )," .
            "sequence_number INT NOT NULL," .
            "first_game_date DATETIME NOT NULL," .
            "last_game_date DATETIME NOT NULL," .
            "last_game_seconds INT NOT NULL," .
            "game_count INT NOT NULL," .
            "game_total_seconds INT NOT NULL," .
            // -1 if not submitted yet
            "review_score TINYINT NOT NULL," .
            "review_name VARCHAR(20) NOT NULL," .
            "review_text TEXT NOT NULL," .
            // stats about player's state when they posted the review
            // they may have played more games since the review
            "review_date DATETIME NOT NULL," .
            "review_game_seconds INT NOT NULL," .
            "review_game_count INT NOT NULL," .
            // in future, we may allow users to upvote/downvote reviews
            "review_votes INT NOT NULL );";

        $result = rs_queryDatabase( $query );

        echo "<B>$tableName</B> table created<BR>";
        }
    else {
        echo "<B>$tableName</B> table already exists<BR>";
        }
    }



function rs_showLog() {
    rs_checkPassword( "show_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."log ".
        "ORDER BY entry_time DESC;";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );



    echo "<a href=\"server.php?action=clear_log\">".
        "Clear log</a>";
        
    echo "<hr>";
        
    echo "$numRows log entries:<br><br><br>\n";
        

    for( $i=0; $i<$numRows; $i++ ) {
        $time = rs_mysqli_result( $result, $i, "entry_time" );
        $entry = htmlspecialchars( rs_mysqli_result( $result, $i, "entry" ) );

        echo "<b>$time</b>:<br>$entry<hr>\n";
        }
    }



function rs_clearLog() {
    rs_checkPassword( "clear_log" );

     echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;

    $query = "DELETE FROM $tableNamePrefix"."log;";
    $result = rs_queryDatabase( $query );
    
    if( $result ) {
        echo "Log cleared.";
        }
    else {
        echo "DELETE operation failed?";
        }
    }




















function rs_logout() {
    rs_checkReferrer();
    
    rs_clearPasswordCookie();

    echo "Logged out";
    }




function rs_showData( $checkPassword = true ) {
    // these are global so they work in embeded function call below
    global $skip, $search, $order_by;

    if( $checkPassword ) {
        rs_checkPassword( "show_data" );
        }
    
    global $tableNamePrefix, $remoteIP;
    

    echo "<table width='100%' border=0><tr>".
        "<td>[<a href=\"server.php?action=show_data" .
            "\">Main</a>] [<a href='server.php?action=regen_static_html'>".
        "Regen HTML</a>]</td>".
        "<td align=right>[<a href=\"server.php?action=logout" .
            "\">Logout</a>]</td>".
        "</tr></table><br><br><br>";




    $skip = rs_requestFilter( "skip", "/[0-9]+/", 0 );
    
    global $usersPerPage;
    
    $search = rs_requestFilter( "search", "/[A-Z0-9_@. \-]+/i" );

    $order_by = rs_requestFilter( "order_by", "/[A-Z_]+/i",
                                  "last_game_date" );
    
    $keywordClause = "";
    $searchDisplay = "";
    
    if( $search != "" ) {
        

        $keywordClause = "WHERE ( email LIKE '%$search%' " .
            "OR id LIKE '%$search%' " .
            "OR review_name LIKE '%$search%' " .
            "OR review_text LIKE '%$search%' ) ";

        $searchDisplay = " matching <b>$search</b>";
        }
    

    

    // first, count results
    $query = "SELECT COUNT(*) FROM $tableNamePrefix".
        "user_stats $keywordClause;";

    $result = rs_queryDatabase( $query );
    $totalRecords = rs_mysqli_result( $result, 0, 0 );


    $orderDir = "DESC";

    if( $order_by == "email" ) {
        $orderDir = "ASC";
        }
    
             
    $query = "SELECT *, ".
        "TIME_TO_SEC(".
        "    TIMEDIFF( last_game_date, first_game_date) ) as play_span, ".
        "ROUND( game_total_seconds / game_count ) as average_game_seconds ".
        "FROM $tableNamePrefix"."user_stats $keywordClause".
        "ORDER BY $order_by $orderDir ".
        "LIMIT $skip, $usersPerPage;";
    $result = rs_queryDatabase( $query );
    
    $numRows = mysqli_num_rows( $result );

    $startSkip = $skip + 1;
    
    $endSkip = $startSkip + $usersPerPage - 1;

    if( $endSkip > $totalRecords ) {
        $endSkip = $totalRecords;
        }



        // form for searching tickets
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

    

    
    echo "$totalRecords user records". $searchDisplay .
        " (showing $startSkip - $endSkip):<br>\n";

    
    $nextSkip = $skip + $usersPerPage;

    $prevSkip = $skip - $usersPerPage;
    
    if( $prevSkip >= 0 ) {
        echo "[<a href=\"server.php?action=show_data" .
            "&skip=$prevSkip&search=$search&order_by=$order_by\">".
            "Previous Page</a>] ";
        }
    if( $nextSkip < $totalRecords ) {
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
    echo "<tr><td>".orderLink( "email", "Email" )."</td>\n";
    echo "<td>".orderLink( "last_game_date", "Last Game" )."</td>\n";
    echo "<td>".orderLink( "last_game_seconds", "Last Game Time" )."</td>\n";
    echo "<td>".orderLink( "game_count", "Game Count" )."</td>\n";
    echo "<td>".orderLink( "game_total_seconds", "Total Game Time" )."</td>\n";
    echo "<td>".orderLink( "average_game_seconds",
                           "Ave Game Time" )."</td>\n";
    echo "<td>".orderLink( "first_game_date", "First Game" )."</td>\n";
    echo "<td>".orderLink( "play_span", "Play Span" )."</td>\n";
    echo "<td>".orderLink( "review_votes", "Review Votes" )."</td>\n";
    echo "<td>Review</td>\n";
    echo "</tr>\n";


    for( $i=0; $i<$numRows; $i++ ) {
        $email = rs_mysqli_result( $result, $i, "email" );
        $last_game_date = rs_mysqli_result( $result, $i, "last_game_date" );
        $last_game_seconds = rs_mysqli_result( $result, $i,
                                              "last_game_seconds" );

        $play_span = rs_mysqli_result( $result, $i, "play_span" );

        $average_game_seconds =
            rs_mysqli_result( $result, $i, "average_game_seconds" );

        
        $game_count = rs_mysqli_result( $result, $i, "game_count" );
        $game_total_seconds = rs_mysqli_result( $result, $i,
                                               "game_total_seconds" );

        $first_game_date = rs_mysqli_result( $result, $i, "first_game_date" );

        $review_votes = rs_mysqli_result( $result, $i, "review_votes" );
        
        $review_text = rs_mysqli_result( $result, $i, "review_text" );

        $review_score = rs_mysqli_result( $result, $i, "review_score" );

        $review_name = rs_mysqli_result( $result, $i, "review_name" );
    

        $lastDuration = rs_secondsToTimeSummary( $last_game_seconds );
        $totalDuration = rs_secondsToTimeSummary( $game_total_seconds );

        $averageDuration = rs_secondsToTimeSummary( $average_game_seconds );

        $spanDuration = rs_secondsToAgeSummary( $play_span );
        
        $lastGameAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                               strtotime( $last_game_date ) );

        $firstGameAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                                strtotime( $first_game_date ) );
        

        $oldLen = strlen( $review_text );
        
        $review_text = substr( $review_text, 0, 80 );

        if( $oldLen > 80 ) {
            $review_text = $review_text . " ...";
            }
        
        $encodedEmail = urlencode( $email );

        $nameString = "";

        if( $review_name != "" ) {
            $nameString = "($review_name) ";
            }
        
        
        echo "<tr>\n";
        
        echo "<td>".
            "<a href=\"server.php?action=show_detail&email=$encodedEmail\">".
            "$email</a></td>\n";
        echo "<td>$last_game_date<br>($lastGameAgo ago)</td>\n";
        echo "<td>$lastDuration</td>\n";
        echo "<td>$game_count</td>\n";
        echo "<td>$totalDuration</td>\n";
        echo "<td>$averageDuration</td>\n";
        echo "<td>$first_game_date<br>($firstGameAgo ago)</td>\n";
        echo "<td>$spanDuration</td>\n";
        echo "<td>$review_votes</td>\n";
        echo "<td>$nameString<b>($review_score)</b><br>$review_text</td>\n";
        echo "</tr>\n";
        }
    echo "</table>";


    echo "<hr>";
    
    echo "<a href=\"server.php?action=show_log\">".
        "Show log</a>";
    echo "<hr>";
    echo "Generated for $remoteIP\n";

    }



function rs_showDetail( $checkPassword = true ) {
    if( $checkPassword ) {
        rs_checkPassword( "show_detail" );
        }
    
    echo "[<a href=\"server.php?action=show_data" .
         "\">Main</a>]<br><br><br>";
    
    global $tableNamePrefix;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i" );
            
    $query = "SELECT * FROM $tableNamePrefix"."user_stats ".
            "WHERE email = '$email';";
    $result = rs_queryDatabase( $query );

    $id = rs_mysqli_result( $result, 0, "id" );
    $email = rs_mysqli_result( $result, 0, "email" );
    $review_date = rs_mysqli_result( $result, 0, "review_date" );

    $review_text = rs_mysqli_result( $result, 0, "review_text" );
    
    $review_score = rs_mysqli_result( $result, 0, "review_score" );
    
    $review_name = rs_mysqli_result( $result, 0, "review_name" );

    echo "<center><table border=0><tr><td>";
    
    echo "<b>Email:</b> $email<br><br>";
    echo "<b>Date:</b> $review_date<br><br>";
    echo "<b>Name:</b> $review_name<br><br>";
    echo "<b>Score:</b> $review_score<br><br>";


    global $fullServerURL;
    
    echo "<b>View:</b> ".
        "<a href='$fullServerURL?action=view_review&id=$id'>[$id]</a><br><br>";
    
    
    $review_text = preg_replace( '/\n/', "<br>", $review_text );
    
    echo "<b>Review:</b><br> ".
        "<table width=600 border=1 cellpadding=5 cellspacing=0><tr><td>$review_text</td></tr></table>";

    echo "</td></tr></table></center>";
    
    
    }



// called from web UI
function rs_regenStaticHTML() {
    rs_checkPassword( "regen_static_html" );

    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();

    rs_showData( false );
    }


// called from cron job
function rs_updateHTML() {
    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();

    echo "OK";
    }



function rs_getReviewHTML( $inID, $inWidth=600, $inTextLengthLimit = -1 ) {
    global $tableNamePrefix;

    $query = "SELECT * FROM $tableNamePrefix"."user_stats ".
            "WHERE id = '$inID';";
    $result = rs_queryDatabase( $query );

    $game_total_seconds = rs_mysqli_result( $result, 0, "game_total_seconds" );
    
    $last_game_date = rs_mysqli_result( $result, 0, "last_game_date" );

    $review_date = rs_mysqli_result( $result, 0, "review_date" );

    $review_text = rs_mysqli_result( $result, 0, "review_text" );
    
    $review_score = rs_mysqli_result( $result, 0, "review_score" );
    
    $review_name = rs_mysqli_result( $result, 0, "review_name" );


    $totalDuration = rs_secondsToTimeSummary( $game_total_seconds );

        
    $lastGameAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                           strtotime( $last_game_date ) );

    $reviewAgo = rs_secondsToAgeSummary( strtotime( "now" ) -
                                         strtotime( $review_date ) );

    global $recIcon, $notRecIcon, $fullServerURL;

    $icon = $recIcon;

    $recText = "Recommended";

    if( $review_score == 0 ) {
        $icon = $notRecIcon;
        $recText = "Not Recommended";
        }

    $text = "<table width=$inWidth cellspacing=0 cellpadding=10 border=0><tr>";


    if( $inWidth >= 500 ) {
        // full width view
        $text= $text.
            "<td valign=middle bgcolor=#404040 width=40>".
            "<a href='$fullServerURL?action=view_review&id=$inID'>".
            "<img src='$icon' border=0></a></td>".
            "<td valign=middle bgcolor=#404040>".
            "<table width=100% cellspacing=0 cellpadding=0><tr>".
            "<td valign=middle><font size=5><b>$recText</b></font></td>".
            "<td align=right>by $review_name<br>".
            "$totalDuration on record</td></tr></table></td></tr>";
        }
    else {
        // compact view
        $text= $text.
            "<td valign=middle bgcolor=#404040 width=40>".
            "<a href='$fullServerURL?action=view_review&id=$inID'>".
            "<img src='$icon' border=0></a></td>".
            "<td valign=middle bgcolor=#404040>".
            "<table width=100% cellspacing=0 cellpadding=0><tr>".
            "<td valign=middle><b>$recText</b><br>".
            "$review_name ".
            "($totalDuration on record)</td></tr></table></td></tr>";
        }
    
    $review_text = preg_replace( '/\n/', "<br>", $review_text );

    $bottomLink = "";
    
    if( $inTextLengthLimit != -1 ) {
        if( strlen( $review_text ) > $inTextLengthLimit ) {
            $review_text = substr( $review_text, 0, $inTextLengthLimit );

            $review_text = $review_text . "...";
            
            global $fullServerURL;
            $bottomLink =
                "<div style='text-align:right'>[<a ".
                "href='$fullServerURL?action=view_review&id=$inID'>".
                "Read more</a>]</div>";
            }
        }
    
    $text = $text .
        "<tr>".
        "<td colspan=2 bgcolor=#202020>".
        "<table border=0 cellpadding=0 cellspacing=0 width=100%>";

    if( $inWidth >= 500 ) {
        $text = $text .
            "<tr><td>Posted $reviewAgo ago</td>".
            "<td align=right>Last Played $lastGameAgo ago</td></tr></table>";
        }
    else {
        $text = $text .
            "<tr><td colspan=2>Posted $reviewAgo ago</td></tr></table>";
        }
    
    $text = $text .
        "<div style='border-top: 2px solid gray'></div><br>".
        "$review_text$bottomLink</td></tr></table>";

    return $text;
    }



// pass in full ORDER BY clause
function rs_getListHTML( $inOrderBy, $inAction, $inWidth = -1,
                         $inSkip=0 ) {
    
    global $tableNamePrefix, $reviewListLength, $summaryTextLength,
        $reviewListWidth;

    
    if( $inWidth == -1 ) {
        $inWidth = $reviewListWidth;
        }
    
    
    $query = "SELECT id FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score != -1 ".
        "$inOrderBy LIMIT $inSkip, $reviewListLength;";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    $totalText = "";
    
    for( $i=0; $i<$numRows; $i++ ) {
        $id = rs_mysqli_result( $result, $i, "id" );

        $text = rs_getReviewHTML( $id, $inWidth, $summaryTextLength );

        $totalText = $totalText . $text . "<br>";

        if( $inWidth >= 500 ) {
            $totalText = $totalText . "<br>";
            }
        }

    // next/prev widget on bottom

    $widgetText = "<table border=0 width=$inWidth><tr>";

    global $fullServerURL;

    $prevShown = false;
    
    if( $inSkip > 0 ) {
        $prev = $inSkip - $reviewListLength;
        if( $prev < 0 ) {
            $prev = 0;
            }
        $widgetText = $widgetText .
            "<td>[<a ".
            "href='$fullServerURL?action=$inAction&skip=$prev'>Prev Page</a>]".
            "</td>";
        
        $prevShown = true;
        }

    $next = $inSkip + $reviewListLength;

    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score != -1 ".
        "$inOrderBy;";
    $result = rs_queryDatabase( $query );

    $count = rs_mysqli_result( $result, 0, 0 );

    if( $count > $next ) {
        if( $prevShown ) {
            $widgetText = $widgetText .
                "<td align=right>[<a ".
                "href='$fullServerURL?action=$inAction&skip=$next'>".
                "Next Page</a>]</td>";
            }
        else {
            $widgetText = $widgetText .
                "<td align=center>[<a ".
                "href='$fullServerURL?action=$inAction&skip=$next'>".
                "More Reviews</a>]</td>";
            }
        }
    
    $widgetText = $widgetText . "</tr></table>";
    
    
    $totalText = $totalText . $widgetText;
    
    
    return $totalText;
    }



function rs_getRecentReviewHTML( $inWidth, $inSkip ) {
    return rs_getListHTML( "ORDER BY review_date DESC",
                           "list_recent", $inWidth, $inSkip );
    }


function rs_getTopPlaytimeReviewHTML( $inWidth, $inSkip ) {
    return rs_getListHTML( "ORDER BY game_total_seconds DESC",
                       "list_playtime", $inWidth, $inSkip );
    }



function rs_listRecent() {
    global $header, $footer;

    eval( $header );
    
    $skip = rs_requestFilter( "skip", "/[0-9]+/i", 0 );
    
    echo "<center><table border=0><tr><td>";

    global $reviewPageWidth;
    
    $text = rs_getRecentReviewHTML( $reviewPageWidth, $skip );

    echo $text;
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }



function rs_listPlaytime() {
    global $header, $footer;

    eval( $header );
    
    $skip = rs_requestFilter( "skip", "/[0-9]+/i", 0 );
    
    echo "<center><table border=0><tr><td>";

    global $reviewPageWidth;
    
    $text = rs_getTopPlaytimeReviewHTML( $reviewPageWidth, $skip );

    echo $text;
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }



// saves HTML to disk
function rs_generateRecentStatic() {
    $text = rs_getRecentReviewHTML( -1, 0 );

    global $outputPathRecent;

    file_put_contents ( $outputPathRecent, $text );
    }


function rs_generateTopPlaytimeStatic() {
    $text = rs_getTopPlaytimeReviewHTML( -1, 0 );

    global $outputPathPlaytime;

    file_put_contents ( $outputPathPlaytime, $text );
    }



function rs_generateReviewCountStatic() {
    global $tableNamePrefix;
    
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score != -1;";
    $result = rs_queryDatabase( $query );

    $count = rs_mysqli_result( $result, 0, 0 );

    
    $query = "SELECT COUNT(*) FROM $tableNamePrefix"."user_stats ".
        "WHERE review_score = 1;";
    $result = rs_queryDatabase( $query );

    
    $posCount = rs_mysqli_result( $result, 0, 0 );

    
    $fraction = $posCount / $count;

    $percent = floor( $fraction * 100 );
    
    $text = "<?php \$rs_reviewCount = $count; ".
        "\$rs_positivePercent = $percent; ?>";

    global $outputReviewCountPHP;

    file_put_contents ( $outputReviewCountPHP, $text );
    }





function rs_viewReview() {
    global $header, $footer;

    eval( $header );
    
    $id = rs_requestFilter( "id", "/[0-9]+/i", 0 );
    $w = rs_requestFilter( "w", "/[0-9]+/i", 600 );

    echo "<center><table border=0><tr><td>";

    global $reviewPageWidth;
    
    $text = rs_getReviewHTML( $id, $reviewPageWidth, -1 );

    echo $text;
    
    echo "</td></tr></table></center>";
    
    eval( $footer );
    }




function rs_getSequenceNumber() {
    global $tableNamePrefix;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );

    if( $email == "" ) {
        rs_log( "getSequenceNumber denied for bad email" );

        echo "DENIED";
        return;
        }
    
    
    $seq = rs_getSequenceNumberForEmail( $email );

    echo "$seq\n"."OK";
    }



// assumes already-filtered, valid email
// returns 0 if not found
function rs_getSequenceNumberForEmail( $inEmail ) {
    global $tableNamePrefix;
    
    $query = "SELECT sequence_number FROM $tableNamePrefix"."user_stats ".
        "WHERE email = '$inEmail';";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );

    if( $numRows < 1 ) {
        return 0;
        }
    else {
        return rs_mysqli_result( $result, 0, "sequence_number" );
        }
    }



function rs_logGame() {
    global $tableNamePrefix, $sharedGameServerSecret;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );
    $game_seconds = rs_requestFilter( "game_seconds", "/[0-9]+/i", "0" );
    $sequence_number = rs_requestFilter( "sequence_number", "/[0-9]+/i", "0" );

    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );


    if( $email == "" ||
        $game_seconds == 0 ) {

        rs_log( "logGame denied for bad email or game_seconds" );
        
        echo "DENIED";
        return;
        }
    
    $trueSeq = rs_getSequenceNumberForEmail( $email );

    if( $trueSeq > $sequence_number ) {
        rs_log( "logGame denied for stale sequence number" );

        echo "DENIED";
        return;
        }

    $computedHashValue =
        strtoupper( rs_hmac_sha1( $sharedGameServerSecret, $sequence_number ) );

    if( $computedHashValue != $hash_value ) {
        rs_log( "logGame denied for bad hash value" );

        echo "DENIED";
        return;
        }

    if( $trueSeq == 0 ) {
        // no record exists, add one
        $query = "INSERT INTO $tableNamePrefix". "user_stats SET " .
            "email = '$email', ".
            "sequence_number = $sequence_number + 1, ".
            "first_game_date = CURRENT_TIMESTAMP, ".
            "last_game_date = CURRENT_TIMESTAMP, ".
            "last_game_seconds = $game_seconds, ".
            "game_count = 1,".
            "game_total_seconds = $game_seconds,".
            "review_score = -1, ".
            "review_name = '',".
            "review_text = '',".
            "review_date = CURRENT_TIMESTAMP,".
            "review_game_seconds = 0,".
            "review_game_count=0,".
            "review_votes = 0;";
        }
    else {
        // update the existing one
        $query = "UPDATE $tableNamePrefix"."user_stats SET " .
            "sequence_number = $sequence_number + 1, ".
            "game_count = game_count + 1, " .
            "game_total_seconds = game_total_seconds + $game_seconds, " .
            "last_game_date = CURRENT_TIMESTAMP, " .
            "last_game_seconds = $game_seconds " .
            "WHERE email = '$email'; ";
        
        }

    rs_queryDatabase( $query );

    // recent reviews won't change, but top playtime might

    // still, regenerate them all so that the Posted On timestamps
    // will update
    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();
    
    echo "OK";
    }







function rs_submitReview() {
    global $tableNamePrefix, $sharedGameServerSecret, $rs_mysqlLink;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );
    $review_score = rs_requestFilter( "review_score", "/[0-9]+/i", "0" );

    $review_name = rs_requestFilter( "review_name", "/[A-Z0-9._\- ]+/i", "" );
    
    $review_text_RAW = $_REQUEST[ "review_text" ];
    
    $slashedText = mysqli_real_escape_string( $rs_mysqlLink, $review_text_RAW );
    
    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );

    
    

    if( $email == "" ) {
        rs_log( "submitReview denied for bad email" );

        echo "DENIED";
        return;
        }


    $textSHA1 = strtoupper( sha1( $review_text_RAW ) );
    $nameSHA1 = strtoupper( sha1( $review_name ) );

    $stringToHash = $review_score . $nameSHA1 . $textSHA1;


    $encodedString = urlencode( $stringToHash );
    $encodedEmail = urlencode( $email );

    global $ticketServerURL;

    $request = "$ticketServerURL".
        "?action=check_ticket_hash".
        "&email=$encodedEmail" .
        "&hash_value=$hash_value" .
        "&string_to_hash=$encodedString";
    
    $result = file_get_contents( $request );

    if( $result != "VALID" ) {
        rs_log( "submitReview denied for failed hash check " );

        echo "DENIED";
        return;
        }

    $query = "SELECT COUNT(*) FROM $tableNamePrefix".
        "user_stats WHERE email = '$email';";

    $result = rs_queryDatabase( $query );
    $count = rs_mysqli_result( $result, 0, 0 );

    if( $count == 0 ) {
        // can post review with no games logged
        $query = "INSERT INTO $tableNamePrefix". "user_stats SET " .
            "email = '$email', ".
            "sequence_number = 1, ".
            "first_game_date = CURRENT_TIMESTAMP, ".
            "last_game_date = CURRENT_TIMESTAMP, ".
            "last_game_seconds = 0, ".
            "game_count = 0,".
            "game_total_seconds = 0,".
            "review_score = $review_score, ".
            "review_name = '$review_name',".
            "review_text = '$slashedText',".
            "review_date = CURRENT_TIMESTAMP,".
            "review_game_seconds = 0,".
            "review_game_count=0,".
            "review_votes = 0;";
        }
    else {
        // update the existing one
        $query = "UPDATE $tableNamePrefix"."user_stats SET " .
            "review_score = $review_score, ".
            "review_name = '$review_name', ".
            "review_text = '$slashedText', ".
            "review_date = CURRENT_TIMESTAMP,".
            "review_game_count = game_count,".
            "review_game_seconds = game_total_seconds ".
            "WHERE email = '$email'; ";
        }
    
    rs_queryDatabase( $query );

    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();
    
    echo "OK";
    }




function rs_removeReview() {
    global $tableNamePrefix, $rs_mysqlLink;
    

    $email = rs_requestFilter( "email", "/[A-Z0-9._%+\-]+@[A-Z0-9.\-]+/i", "" );

    $sequence_number = rs_requestFilter( "sequence_number", "/[0-9]+/i", "0" );

    $hash_value = rs_requestFilter( "hash_value", "/[A-F0-9]+/i", "" );

    $hash_value = strtoupper( $hash_value );
    

    if( $email == "" ) {
        rs_log( "removeReview denied for bad email" );

        echo "DENIED";
        return;
        }
    
    $trueSeq = rs_getSequenceNumberForEmail( $email );

    if( $trueSeq > $sequence_number || $trueSeq == 0 ) {
        // stale sequence number or user doesn't exist in DB
        rs_log( "removeReview denied for stale sequence number" );

        echo "DENIED";
        return;
        }
    

    $stringToHash = $sequence_number;


    $encodedString = urlencode( $stringToHash );
    $encodedEmail = urlencode( $email );

    global $ticketServerURL;
    
    $result = file_get_contents(
            "$ticketServerURL".
            "?action=check_ticket_hash".
            "&email=$encodedEmail" .
            "&hash_value=$hash_value" .
            "&string_to_hash=$encodedString" );

    if( $result != "VALID" ) {
        rs_log( "removeReview denied for failed hash check" );

        echo "DENIED";
        return;
        }

    // just set score as -1
    // keep old review text in place for reference
    
    $query = "UPDATE $tableNamePrefix"."user_stats SET " .
        "sequence_number = $sequence_number + 1, ".
        "review_score = -1 ".
        "WHERE email = '$email'; ";

    rs_queryDatabase( $query );

    rs_generateRecentStatic();
    rs_generateTopPlaytimeStatic();
    rs_generateReviewCountStatic();
    
    echo "OK";
    }



$rs_mysqlLink;


// general-purpose functions down here, many copied from seedBlogs

/**
 * Connects to the database according to the database variables.
 */  
function rs_connectToDatabase() {
    global $databaseServer,
        $databaseUsername, $databasePassword, $databaseName,
        $rs_mysqlLink;
    
    
    $rs_mysqlLink =
        mysqli_connect( $databaseServer, $databaseUsername, $databasePassword )
        or rs_operationError( "Could not connect to database server: " .
                              mysqli_error( $rs_mysqlLink ) );
    
    mysqli_select_db( $rs_mysqlLink, $databaseName )
        or rs_operationError( "Could not select $databaseName database: " .
                              mysqli_error( $rs_mysqlLink ) );
    }


 
/**
 * Closes the database connection.
 */
function rs_closeDatabase() {
    global $rs_mysqlLink;
    
    mysqli_close( $rs_mysqlLink );
    }


/**
 * Returns human-readable summary of a timespan.
 * Examples:  10.5 hours
 *            34 minutes
 *            45 seconds
 */
function rs_secondsToTimeSummary( $inSeconds ) {
    if( $inSeconds < 120 ) {
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
function rs_secondsToAgeSummary( $inSeconds ) {
    if( $inSeconds < 120 ) {
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
function rs_queryDatabase( $inQueryString ) {
    global $rs_mysqlLink;
    
    if( gettype( $rs_mysqlLink ) != "resource" ) {
        // not a valid mysql link?
        rs_connectToDatabase();
        }
    
    $result = mysqli_query( $rs_mysqlLink, $inQueryString );
    
    if( $result == FALSE ) {

        $errorNumber = mysqli_errno( $rs_mysqlLink );
        
        // server lost or gone?
        if( $errorNumber == 2006 ||
            $errorNumber == 2013 ||
            // access denied?
            $errorNumber == 1044 ||
            $errorNumber == 1045 ||
            // no db selected?
            $errorNumber == 1046 ) {

            // connect again?
            rs_closeDatabase();
            rs_connectToDatabase();

            $result = mysqli_query( $rs_mysqlLink, $inQueryString )
                or rs_operationError(
                    "Database query failed:<BR>$inQueryString<BR><BR>" .
                    mysqli_error( $rs_mysqlLink ) );
            }
        else {
            // some other error (we're still connected, so we can
            // add log messages to database
            rs_fatalError( "Database query failed:<BR>$inQueryString<BR><BR>" .
                           mysqli_error( $rs_mysqlLink ) );
            }
        }

    return $result;
    }



/**
 * Replacement for the old mysql_result function.
 */
function rs_mysqli_result( $result, $number, $field=0 ) {
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
function rs_doesTableExist( $inTableName ) {
    // check if our table exists
    $tableExists = 0;
    
    $query = "SHOW TABLES";
    $result = rs_queryDatabase( $query );

    $numRows = mysqli_num_rows( $result );


    for( $i=0; $i<$numRows && ! $tableExists; $i++ ) {

        $tableName = rs_mysqli_result( $result, $i, 0 );
        
        if( $tableName == $inTableName ) {
            $tableExists = 1;
            }
        }
    return $tableExists;
    }



function rs_log( $message ) {
    global $enableLog, $tableNamePrefix, $rs_mysqlLink;

    if( $enableLog ) {
        $slashedMessage = mysqli_real_escape_string( $rs_mysqlLink, $message );
    
        $query = "INSERT INTO $tableNamePrefix"."log VALUES ( " .
            "'$slashedMessage', CURRENT_TIMESTAMP );";
        $result = rs_queryDatabase( $query );
        }
    }



/**
 * Displays the error page and dies.
 *
 * @param $message the error message to display on the error page.
 */
function rs_fatalError( $message ) {
    //global $errorMessage;

    // set the variable that is displayed inside error.php
    //$errorMessage = $message;
    
    //include_once( "error.php" );

    // for now, just print error message
    $logMessage = "Fatal error:  $message";
    
    echo( $logMessage );

    rs_log( $logMessage );
    
    die();
    }



/**
 * Displays the operation error message and dies.
 *
 * @param $message the error message to display.
 */
function rs_operationError( $message ) {
    
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
function rs_addslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'rs_addslashes_deep', $inValue )
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
function rs_stripslashes_deep( $inValue ) {
    return
        ( is_array( $inValue )
          ? array_map( 'rs_stripslashes_deep', $inValue )
          : stripslashes( $inValue ) );
    }



/**
 * Filters a $_REQUEST variable using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function rs_requestFilter( $inRequestVariable, $inRegex, $inDefault = "" ) {
    if( ! isset( $_REQUEST[ $inRequestVariable ] ) ) {
        return $inDefault;
        }

    return rs_filter( $_REQUEST[ $inRequestVariable ], $inRegex, $inDefault );
    }


/**
 * Filters a value  using a regex match.
 *
 * Returns "" (or specified default value) if there is no match.
 */
function rs_filter( $inValue, $inRegex, $inDefault = "" ) {
    
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
function rs_checkPassword( $inFunctionName ) {
    $password = "";
    $password_hash = "";

    $badCookie = false;
    
    
    global $accessPasswords, $tableNamePrefix, $remoteIP, $enableYubikey,
        $passwordHashingPepper;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    $passwordSent = false;
    
    if( isset( $_REQUEST[ "passwordHMAC" ] ) ) {
        $passwordSent = true;

        // already hashed client-side on login form
        // hash again, because hash client sends us is not stored in
        // our settings file
        $password = rs_hmac_sha1( $passwordHashingPepper,
                                  $_REQUEST[ "passwordHMAC" ] );
        
        
        // generate a new hash cookie from this password
        $newSalt = time();
        $newHash = md5( $newSalt . $password );
        
        $password_hash = $newSalt . "_" . $newHash;
        }
    else if( isset( $_COOKIE[ $cookieName ] ) ) {
        rs_checkReferrer();
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

            rs_log( "Failed $inFunctionName access with password:  ".
                    "$password" );
            }
        else {
            echo "Session expired.";
                
            rs_log( "Failed $inFunctionName access with bad cookie:  ".
                    "$password_hash" );
            }
        
        die();
        }
    else {
        
        if( $passwordSent && $enableYubikey ) {
            global $yubikeyIDs, $yubicoClientID, $yubicoSecretKey,
                $ticketGenerationSecret;
            
            $yubikey = $_REQUEST[ "yubikey" ];

            $index = array_search( $password, $accessPasswords );
            $yubikeyIDList = preg_split( "/:/", $yubikeyIDs[ $index ] );

            $providedID = substr( $yubikey, 0, 12 );

            if( ! in_array( $providedID, $yubikeyIDList ) ) {
                echo "Provided Yubikey does not match ID for this password.";
                die();
                }
            
            
            $nonce = rs_hmac_sha1( $ticketGenerationSecret, uniqid() );
            
            $callURL =
                "http://api2.yubico.com/wsapi/2.0/verify?id=$yubicoClientID".
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
 



function rs_clearPasswordCookie() {
    global $tableNamePrefix;

    $cookieName = $tableNamePrefix . "cookie_password_hash";

    // expire 24 hours ago (to avoid timezone issues)
    $expireTime = time() - 60 * 60 * 24;

    setcookie( $cookieName, "", $expireTime, "/" );
    }
 
 







function rs_hmac_sha1( $inKey, $inData ) {
    return hash_hmac( "sha1", 
                      $inData, $inKey );
    } 

 
function rs_hmac_sha1_raw( $inKey, $inData ) {
    return hash_hmac( "sha1", 
                      $inData, $inKey, true );
    } 


 
 
 
 
// decodes a ASCII hex string into an array of 0s and 1s 
function rs_hexDecodeToBitString( $inHexString ) {
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
 


 
?>
